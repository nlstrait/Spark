/*
  ==============================================================================

    AudioRecorder.h
    Created: 8 May 2021
    Author:  Nolan Strait
 
    Adapted from JUCE's Audio Recording Demo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


/** A simple class that acts as an AudioIODeviceCallback and writes the
    incoming audio data to a WAV file.
*/
class AudioRecorder : public juce::AudioIODeviceCallback {
public:
    /**
     Creates a new AudioRecorder.
     @param thumbnailToUpdate   A thumbnail for this AudioRecorder to update as it records.
     */
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
    
    /**
     Prepare this AudioRecorder for recording.
     @param device  The audio device which this AudioRecorder will record input from.
     */
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    
    /**
    Callback function for the audio device which this AudioRecorder is recording input from. This is where the real audio processing happens.
     @param inputChannelData         A pointer to a multi-dimensional array of input channel data.
     @param numInputChannels         The number of input channels; determines dimensionality of input channel data array.
     @param outputChannelData       A pointer to a multi-dimensional array of output channel data.
     @param numOutputChannels       The number of output channels; determines dimensionality of output channel data array.
     @param numSamples                       The size of the block of audio samples to be processed.
     */
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


/**
 A component for visualizing audio as it is being recorded.
 */
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
     @param displayFull     True to display fullthumbnail.
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


/**
 This visualizer component takes audio input and creates a display of a scrolling audio waveform.
 */
class LiveScrollingAudioDisplay : public juce::AudioVisualiserComponent, public juce::AudioIODeviceCallback {
public:
    LiveScrollingAudioDisplay();
    
    /**
     Prepare this LiveScrollingAudioDisplay for displaying.
     @param device  The audio device which this LiveScrollingAudioDisplay will display input from.
     */
    void audioDeviceAboutToStart(juce::AudioIODevice*) override;
    void audioDeviceStopped() override;
    
    /**
    Callback function for the audio device which this LiveScrollingAudioDisplay is recording input from. This is where the real audio processing happens.
     @param inputChannelData         A pointer to a multi-dimensional array of input channel data.
     @param numInputChannels         The number of input channels; determines dimensionality of input channel data array.
     @param outputChannelData       A pointer to a multi-dimensional array of output channel data.
     @param numOutputChannels       The number of output channels; determines dimensionality of output channel data array.
     @param numSamples                       The size of the block of audio samples to be processed.
     */
    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numberOfSamples) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveScrollingAudioDisplay)
};


/**
 The top-level component which houses the other components necessary for recording audio.
 */
class AudioRecorderComponent : public juce::Component {
public:
    /**
     Create a new AudioRecorderComponent
     @param adm     The audio device manager which this AudioRecorderComponent will use to recieve audio input and send audio output from selected audio devices.
     */
    AudioRecorderComponent(juce::AudioDeviceManager& adm);
    ~AudioRecorderComponent() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    juce::AudioDeviceManager& audioDeviceManager;
    
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




