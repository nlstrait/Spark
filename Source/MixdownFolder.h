/*
  ==============================================================================

    MixdownFolder.h
    Created: 11 May 2021
    Author: Tuan Thai

    Initializes variables and creates interfaces for class functions.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MixdownFolderComp :   public juce::Component,
                            public juce::AudioSource,
                            public juce::ChangeListener {

/*
  ==============================================================================

        Public functions of Mixdown Folder

  ==============================================================================
*/

public:

    /**
    * Acts as constructor to set up necessary variables and functions.
    */
    MixdownFolderComp(juce::AudioDeviceManager& adm);

    /**
    * Mixdown folder destructor.
    */
    ~MixdownFolderComp() override;

    /**
    * Function sets up the transport source.
    *
    * @param samplesPerBlockExpected  Number of samples per block to be loaded in.
    * @param sampleRate  Expected sample rate of files.
    */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    /*
    * Function gets the next audio block of the current audio file.
    *
    * @param bufferToFill  A buffer that holds audio blocks from a selected audio file, in form
    *   AudioSourceChannelInfo object.
    */
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    /**
    * Function releases resources when they are no longer needed.
    */
    void releaseResources() override;

    /**
    * Function redraws parts of component that require updates.
    * Called when a part of the MixDownFolder component requires redrawing.
    *
    * @param g  The graphics context passed to this class for any drawing.
    */
    void paint (juce::Graphics&) override;

    /**
    * Function resizes the dimensions of component dynamically to parent size.
    * Called everytime the MixDownFolder component size is changed.
    */
    void resized() override;

 /*
   ==============================================================================

        Private functions of Mixdown Folder

   ==============================================================================
 */

private:
    //Device manager for user output/input
    juce::AudioDeviceManager& deviceManager;

    //States of Mixdown Folder component for playback and selection
    enum TransportState {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    TransportState state;

    //Audio format manager, audio transport source, and audio format reader source for
    //formatting and preparing audio files for playback
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> reader;
    juce::AudioTransportSource transport;

    /**
    * Callback function that projects changes audio transport source.
    * Changes come in the form of state changes.
    *
    * @param source  Changebroadcaster that sends messages to related classes.
    */
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    /**
    * Function facilitates state change via changing button text and availability,
    * as well as sets playback track time position.
    * state.Stopped represents an unplayed track or unloaded state.
    * state.Starting represents playing an unplayed track.
    * state.Playing represents a currently played track.
    * state.Pausing represents pausing a currently played track.
    * state.Paused represents a currently paused track.
    *
    * @param newState  Accepts a TransportState that is an enumeration of MixdownFolder states.
    */
    void stateChange(TransportState newState);

    //Saved audio files for id tracking and file storage
    juce::Array<juce::File> audioFiles;
    juce::File selectedAudioFile;

    //Audio file storage and event response
    juce::ComboBox fileBoxMenu;
    void fileBoxMenuChanged();

    //File query system and event response
    juce::TextButton fileButton;
    void fileButtonClickResponse();

    //Next file selection and event response
    juce::TextButton nextButton;
    void nextButtonClickResponse();

    //Previous file selection and event response
    juce::TextButton prevButton;
    void prevButtonClickResponse();

    //Play current file selected and event response
    juce::TextButton playButton;
    void playButtonClickResponse();

    //Stop current selected file playback and event response
    juce::TextButton stopButton;
    void stopButtonClickResponse();

    //Setup Macro to keep a certain coding style standard necessary for JUCE
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixdownFolderComp)
};
