#pragma once

#include <JuceHeader.h>
#include "AudioRecorder.h"
#include "AudioPlayback.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  :  public juce::AudioAppComponent,
                        public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void prepareToPlay(int, double) override {}
    
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    void releaseResources() override {}

private:
    //==============================================================================
    // Your private member variables go here...
    juce::AudioDeviceSelectorComponent audioSetupComp;
    AudioRecorderComponent recorderComponent;
    juce::TextEditor diagnosticsBox; // mostly for debugging purposes
    
    DemoThumbnailComponent demoThumbComp;
    juce::AudioFormatManager formatManager;
    
    void changeListenerCallback(juce::ChangeBroadcaster*) override { dumpDeviceInfo(); }
    
    void dumpDeviceInfo();
    void logMessage(const juce::String& m);
    
    juce::Random random; // for adding noise to prevent feedback

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
