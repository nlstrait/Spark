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
#include "ThumbnailComp.h"
#include "DemoUtilities.h"

class MixdownFolderComp :   public juce::Component,
                            public juce::AudioSource,
                            public juce::Slider::Listener,
#if (JUCE_ANDROID || JUCE_IOS)
    private Button::Listener,
#else
    private juce::FileBrowserListener,
#endif
    private juce::ChangeListener
{

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
    * Slider type : Audio Position Slider
    * Function changes the time positioning of the current audio file based upon the slider drag.
    * Called when audioPositionSlider is dragged.
    */
    void sliderValueChanged(juce::Slider* slider) override;

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
    std::unique_ptr<ThumbnailComp> tn;
    juce::Slider zoomSlider{ Slider::LinearHorizontal, Slider::NoTextBox };
    URL currentAudioFileURL;
    TimeSliceThread thread{ "audio file preview" };

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

    //Slider to modify audio transport position
    juce::Slider audioPositionSlider;
    juce::Label sliderLabel;

    //Play current file selected and event response
    juce::TextButton playButton;
    void playButtonClickResponse();

    //Stop current selected file playback and event response
    juce::TextButton stopButton;
    void stopButtonClickResponse();



    void showAudioResource(URL resource)
    {
        if (loadURLIntoTransport(resource))
            currentAudioFileURL = std::move(resource);

        zoomSlider.setValue(0, dontSendNotification);
        tn->setURL(currentAudioFileURL);
    }

    bool loadURLIntoTransport(const URL& audioURL)
    {
        transport.stop();
        transport.setSource(nullptr);
        reader.reset();

        AudioFormatReader* reader2 = nullptr;

    #if ! JUCE_IOS
        if (audioURL.isLocalFile())
        {
            reader2 = audioFormatManager.createReaderFor(audioURL.getLocalFile());
        }
        else
    #endif
        {
            if (reader2 == nullptr)
                reader2 = audioFormatManager.createReaderFor(audioURL.createInputStream(false));
        }

        if (reader2 != nullptr)
        {
            reader.reset(new AudioFormatReaderSource(reader2, true));

            transport.setSource(reader.get(),
                32768,                   
                &thread,              
                reader2->sampleRate);     

            return true;
        }

        return false;
    }

    void startOrStop()
    {
        if (transport.isPlaying())
        {
            transport.stop();
        }
        else
        {
            transport.setPosition(0);
            transport.start();
        }
    }

    void selectionChanged() override {
        showAudioResource(URL(tn->getLastDroppedFile()));
    }

    void fileClicked(const File&, const MouseEvent&) override {}

    void fileDoubleClicked(const File&) override {}

    void browserRootChanged(const File&) override {}

    //Setup Macro to keep a certain coding style standard necessary for JUCE
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixdownFolderComp)
};
