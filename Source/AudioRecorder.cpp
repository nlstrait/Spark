/*
  ==============================================================================

    AudioRecorder.cpp
    Created: 8 May 2021
    Author:  Nolan Strait
 
    Adapted from JUCE's Audio Recording Demo

  ==============================================================================
*/

#include "AudioRecorder.h"
#include "MixdownFolder.h"


//===================================== AudioRecorder =========================================

AudioRecorder::AudioRecorder(juce::AudioThumbnail& thumbnailToUpdate)  : thumbnail(thumbnailToUpdate) {
    backgroundThread.startThread();
}

AudioRecorder::~AudioRecorder() {
    stopRecording();
}

void AudioRecorder::startRecording(const juce::File& file, double paddingTime) {
    stopRecording();
    if (sampleRate <= 0) return;
    file.deleteFile();
    
    // Open filestream to write to destination file
    if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream())) {
        // Writes to output stream in correct format
        juce::WavAudioFormat wavFormat;
        
        if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 1, 16, {}, 0)) {
            fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it
            
            if (paddingTime > 0.0) padRecording(writer, paddingTime);
            
            // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
            // write the data to disk on our background thread.
            threadedWriter.reset (new juce::AudioFormatWriter::ThreadedWriter (writer, backgroundThread, 32768)); // Why this buffer size? Is this a randomly large number? @Nolan
            
            // Reset our recording thumbnail (which is visualizing our audio input waveform)
            thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
            nextSampleNum = 0;
            
            // And now, swap over our active writer pointer so that the audio callback will start using it.
            const juce::ScopedLock sl (writerLock);
            activeWriter = threadedWriter.get();
        }
    } else throw "Unable to open provided file";
}

void AudioRecorder::padRecording(juce::AudioFormatWriter* writer, double paddingTime) {
    int numSamples = paddingTime * writer->getSampleRate();
    juce::AudioBuffer<float> padding(writer->getNumChannels(), numSamples);
    padding.clear();
    writer->writeFromAudioSampleBuffer(padding, 0, numSamples);
}

void AudioRecorder::stopRecording() {
    // First, clear this pointer to stop the audio callback from using our writer object..
    {
        const juce::ScopedLock sl (writerLock);
        activeWriter = nullptr;
    }

    // Now we can delete the writer object. It's done in this order because the deletion could
    // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
    // the audio callback while this happens.
    threadedWriter.reset();
}

bool AudioRecorder::isRecording() const {
    return activeWriter.load() != nullptr;
}

void AudioRecorder::audioDeviceAboutToStart(juce::AudioIODevice *device) {
    sampleRate = device->getCurrentSampleRate();
}

void AudioRecorder::audioDeviceStopped() {
    sampleRate = 0;
}

void AudioRecorder::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                            float** outputChannelData, int numOutputChannels,
                            int numSamples) {
    
    const juce::ScopedLock sl(AudioRecorder::writerLock);

    if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels()) // This seems funky; why would we compare numInput channels with the thumbnail channels? I thought the thumbnail was just used for painting waveforms @Nolan
    {
        activeWriter.load()->write (inputChannelData, numSamples);

        // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
        juce::AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);
        thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
        nextSampleNum += numSamples;
    }

    // We need to clear the output buffers, in case they're full of junk..
    // Why do we need to do this? @Nolan
    //
    // TODO: Confirm that buffer clearance does not interfere with other components
    for (int i = 0; i < numOutputChannels; ++i)
        if (outputChannelData[i] != nullptr)
            juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
}


//===================================== RecordingThumbnail =========================================

RecordingThumbnail::RecordingThumbnail() {
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

RecordingThumbnail::~RecordingThumbnail() {
    thumbnail.removeChangeListener(this);
}

juce::AudioThumbnail& RecordingThumbnail::getAudioThumbnail() { return thumbnail; }

void RecordingThumbnail::setDisplayFullThumbnail(bool displayFull) {
    displayFullThumb = displayFull;
    repaint();
}

void RecordingThumbnail::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightgrey);
    
    // If our thumbnail has length (has a waveform to display)...
    if (thumbnail.getTotalLength() > 0.0) {
        auto endTime = displayFullThumb ? thumbnail.getTotalLength() : juce::jmax(30.0, thumbnail.getTotalLength());
        auto thumbArea = getLocalBounds();
        thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0f);
    } else {
        g.setFont(14.0f);
        g.drawFittedText ("(No file recorded)", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void RecordingThumbnail::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &thumbnail) repaint();
}


//===================================== LiveScrollingAudioDisplay =========================================

LiveScrollingAudioDisplay::LiveScrollingAudioDisplay() : juce::AudioVisualiserComponent(1) {
    // TODO: May need alignment with selected device from AudioDeviceManager
    setSamplesPerBlock(256);
    setBufferSize(1024);
}

void LiveScrollingAudioDisplay::audioDeviceAboutToStart(juce::AudioIODevice *) { clear(); }

void LiveScrollingAudioDisplay::audioDeviceStopped() { clear(); }

void LiveScrollingAudioDisplay::audioDeviceIOCallback(const float **inputChannelData, int numInputChannels,
                                                      float **outputChannelData, int numOutputChannels,
                                                      int numberOfSamples) {
    for (int i = 0; i < numberOfSamples; ++i) {
        
        float inputSample = 0;

        for (int chan = 0; chan < numInputChannels; ++chan)
            if (const float* inputChannel = inputChannelData[chan])
                inputSample += inputChannel[i];  // find the sum of all the channels

        inputSample *= 3.0f; // boost the level to make it more easily visible.

        pushSample (&inputSample, 1);
    }

    // We need to clear the output buffers before returning, in case they're full of junk..
    for (int j = 0; j < numOutputChannels; ++j)
        if (float* outputChannel = outputChannelData[j])
            juce::zeromem (outputChannel, (size_t) numberOfSamples * sizeof (float));
}


//===================================== Utilities =========================================

//inline juce::Colour getUIColourIfAvailable (juce::LookAndFeel_V4::ColourScheme::UIColour uiColour, juce::Colour fallback = juce::Colour (0xff4d4d4d)) noexcept {
//    if (auto* v4 = dynamic_cast<juce::LookAndFeel_V4*> (&juce::LookAndFeel::getDefaultLookAndFeel()))
//        return v4->getCurrentColourScheme().getUIColour (uiColour);
//
//    return fallback;
//}


//===================================== LayerRecorderComponent =========================================

LayerRecorderComponent::LayerRecorderComponent(juce::AudioDeviceManager& adm) : audioDeviceManager(adm), currProject(nullptr), transport(nullptr) {
    setOpaque (true);
    //addAndMakeVisible (liveAudioScroller);

    addAndMakeVisible (explanationLabel);
    explanationLabel.setFont (juce::Font (15.0f, juce::Font::plain));
    explanationLabel.setJustificationType (juce::Justification::topLeft);
    explanationLabel.setEditable (false, false, false);
    explanationLabel.setColour (juce::TextEditor::textColourId, juce::Colours::black);
    explanationLabel.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    addAndMakeVisible (recordButton);
    recordButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffff5c5c));
    recordButton.setColour (juce::TextButton::textColourOnId, juce::Colours::black);

    recordButton.onClick = [this] {
        if (recorder.isRecording()) stopRecording();
        else startRecording();
    };
    recordButton.setEnabled(false); // disabled until a project is loaded

    addAndMakeVisible (recordingThumbnail);

    juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                 [this] (bool granted) {
                                     int numInputChannels = granted ? 2 : 0;
                                     audioDeviceManager.initialise (numInputChannels, 2, nullptr, true, {}, nullptr);
                                 });

    audioDeviceManager.addAudioCallback (&liveAudioScroller);
    audioDeviceManager.addAudioCallback (&recorder);

    setSize (500, 500);
}

LayerRecorderComponent::~LayerRecorderComponent() {
    audioDeviceManager.removeAudioCallback (&recorder);
    audioDeviceManager.removeAudioCallback (&liveAudioScroller);
}

void LayerRecorderComponent::paint(juce::Graphics &g) {
    g.fillAll(getUIColourIfAvailable(juce::LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
}

void LayerRecorderComponent::resized() {
    auto area = getLocalBounds();

    //liveAudioScroller .setBounds (area.removeFromTop (80).reduced (8));
    recordingThumbnail.setBounds (area.removeFromTop (80).reduced (8));
    recordButton      .setBounds (area.removeFromTop (36).removeFromLeft (140).reduced (8));
    explanationLabel  .setBounds (area.reduced (8));
}

void LayerRecorderComponent::setProject(Project *p) {
    this->currProject = p;
    recordButton.setEnabled(true);
    explanationLabel.setText("Press to record a new layer for project " + p->getName(), juce::sendNotification);
}

void LayerRecorderComponent::setTransport(juce::AudioTransportSource* ats) {
    this->transport = ats;
}
void LayerRecorderComponent::setPlaybackComp(MixdownFolderComp* playbackComp) {
    this->playbackComp = playbackComp;
}

void LayerRecorderComponent::startRecording() {
    if (! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::writeExternalStorage)) {
        SafePointer<LayerRecorderComponent> safeThis (this);

        juce::RuntimePermissions::request (juce::RuntimePermissions::writeExternalStorage,
                                     [safeThis] (bool granted) mutable {
                                         if (granted) safeThis->startRecording();
                                     });
        return;
    }

    Layer newLayer = currProject->createNewLayer();
    recorder.startRecording (newLayer.getFile(), transport->getCurrentPosition());
    isCurrentlyRecording = true;
    playbackComp->triggerPlayback();

    recordButton.setButtonText ("Stop");
    recordingThumbnail.setDisplayFullThumbnail (false);
}

void LayerRecorderComponent::stopRecording() {
    recorder.stopRecording();
    isCurrentlyRecording = false;
    recordButton.setButtonText ("Record");
    recordingThumbnail.setDisplayFullThumbnail (true);
}
