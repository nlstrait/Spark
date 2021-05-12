/*
  ==============================================================================

    MixdownFolder.h
    Created: 11 May 2021 3:19:48pm
    Author:  

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MixdownFolderComp :   public juce::Component,
                            public juce::AudioSource,
                            public juce::ChangeListener {
public:
    //==============================================================================
    MixdownFolderComp(juce::AudioDeviceManager& adm);
    ~MixdownFolderComp() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioDeviceManager& deviceManager;

    enum TransportState {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    TransportState state;

    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> reader;
    juce::AudioTransportSource transport;
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void stateChange(TransportState newState);

    juce::Array<juce::File> audioFiles;
    juce::File selectedAudioFile;

    juce::ComboBox fileBoxMenu;
    void fileBoxMenuChanged();

    juce::TextButton fileButton;
    void fileButtonClickResponse();

    juce::TextButton nextButton;
    void nextButtonClickResponse();

    juce::TextButton prevButton;
    void prevButtonClickResponse();

    juce::TextButton playButton;
    void playButtonClickResponse();

    juce::TextButton stopButton;
    void stopButtonClickResponse();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixdownFolderComp)
};
