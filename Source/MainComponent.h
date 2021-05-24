#pragma once

#include <JuceHeader.h>
#include "AudioRecorder.h"
#include "MixdownFolder.h"

//==============================================================================
/**
    This component lives inside our window, and this is where you should put all
    your controls and content. This component acts as a place where all of the
    components are assembled.
*/

class MainComponent  :  public juce::AudioAppComponent,
                        public juce::ChangeListener
{
public:
    //==============================================================================

    /**
    * MainComponent constructor
    */
    MainComponent();

    /**
    * MainComponent destructor
    */
    ~MainComponent() override;

    //==============================================================================

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
    
    /**
    * Function sets up the transport source.
    *
    * @param samplesPerBlockExpected  Number of samples per block to be loaded in.
    * @param sampleRate  Expected sample rate of files.
    */
    void prepareToPlay(int, double) override;
    
    /**
    * Function gets the next audio block of the current audio file.
    *
    * @param bufferToFill  A buffer that holds audio blocks from a selected audio file, in form
    *   AudioSourceChannelInfo object.
    */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    /**
    * Releases the resources when no longer needed.
    * Not used within MainComponent.cpp
    */
    void releaseResources() override {}

private:
    //==============================================================================

    /*
    * These are the components that all make up the JUCE main component
    */
    juce::AudioDeviceSelectorComponent audioSetupComp;
    AudioRecorderComponent recorderComponent;
    MixdownFolderComp mixdownFolderComp;
    juce::TextEditor diagnosticsBox;
    
    /*
    * Callback that catches component events to trigger a function cascade
    */
    void changeListenerCallback(juce::ChangeBroadcaster*) override { dumpDeviceInfo(); }
    
    /**
    * Function logs current adapted audio device information to "diagnosticsBox" text box.
    */
    void dumpDeviceInfo();

    /**
    * Logs audio device diagnostics in form "^Diagnostics information here"
    * @param m  A string to be converted to log message
    */
    void logMessage(const juce::String& m);
    
    // for adding noise to prevent feedback
    juce::Random random;

    //Macro to set up style guidelines for necessary JUCE functions
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
