
// TODO: Some private member variables may be able to be pushed down into the .cpp file

#pragma once

#include <JuceHeader.h>


/** A simple class that acts as an AudioIODeviceCallback and writes the
    incoming audio data to a WAV file.
*/
class AudioRecorder : public juce::AudioIODeviceCallback {
public:
    AudioRecorder(juce::AudioThumbnail& thumbnailToUpdate);
    ~AudioRecorder() override;
    
    /**
     Begin recording.
     @param file    File to record to.
     */
    void startRecording(const juce::File& file);
    
    /**
     Stop recording.
     */
    void stopRecording();
    
    /**
     Determine if this AudioRecorder is currently recording.
     @return True if recording
     */
    bool isRecording() const;
    
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels,
                                int numSamples) override;
    
private:
    juce::AudioThumbnail& thumbnail; // for drawing scaled view of audio waveform
    juce::TimeSliceThread backgroundThread { "Audio Recorder Thread" }; // this thread writes audio data to disk
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter; // FIFO buffer for incoming data
    double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;
    
    // Thread safety
    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};



class RecordingThumbnail : public juce::Component, private juce::ChangeListener {
public:
    RecordingThumbnail();
    ~RecordingThumbnail() override;
    
    /**
     Get this RecordingThumbnail's audio thumbnail.
     @return A pointer to an audio thumbnail.
     */
    juce::AudioThumbnail& getAudioThumbnail();
    
    /**
     Display full thumbnail.
     */
    void setDisplayFullThumbnail(bool displayFull);
    
    void paint(juce::Graphics& g) override;

private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache  { 10 };
    juce::AudioThumbnail thumbnail            { 512, formatManager, thumbnailCache };

    bool displayFullThumb = false;

    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecordingThumbnail)
    
};



class LiveScrollingAudioDisplay : public juce::AudioVisualiserComponent, public juce::AudioIODeviceCallback {
public:
    LiveScrollingAudioDisplay();
    
    void audioDeviceAboutToStart(juce::AudioIODevice*) override;
    void audioDeviceStopped() override;
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numberOfSamples) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveScrollingAudioDisplay)
};



class AudioRecorderComponent : public juce::Component {
public:
    AudioRecorderComponent();
    ~AudioRecorderComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::AudioDeviceManager audioDeviceManager; // TODO: Confirm functionality
    
    LiveScrollingAudioDisplay liveAudioScroller;
    RecordingThumbnail recordingThumbnail;
    AudioRecorder recorder { recordingThumbnail.getAudioThumbnail() };
    
    juce::Label explanationLabel { {}, "Record a wave file from the live audio input.\n\nPressing record will start recording a file in your \"Documents\" folder."};
    juce::TextButton recordButton { "Record" };
    juce::File lastRecording;
    
    void startRecording();
    
    void stopRecording();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioRecorderComponent)
};




