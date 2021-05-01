/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioDeviceManagerTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Explores features of the audio device manager.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make, xcode_iphone, androidstudio

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
class MainContentComponent   : public juce::AudioAppComponent,
                               public juce::ChangeListener,
                               private juce::Timer
{
public:
    //==============================================================================
    MainContentComponent()
        : audioSetupComp (deviceManager,
                          0,     // minimum input channels
                          256,   // maximum input channels
                          0,     // minimum output channels
                          256,   // maximum output channels
                          false, // ability to select midi inputs
                          false, // ability to select midi output device
                          false, // treat channels as stereo pairs
                          false) // hide advanced options
    {
        
        addAndMakeVisible (audioSetupComp);
        addAndMakeVisible (diagnosticsBox);

        diagnosticsBox.setMultiLine (true);
        diagnosticsBox.setReturnKeyStartsNewLine (true);
        diagnosticsBox.setReadOnly (true);
        diagnosticsBox.setScrollbarsShown (true);
        diagnosticsBox.setCaretVisible (false);
        diagnosticsBox.setPopupMenuEnabled (true);
        diagnosticsBox.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x32ffffff));
        diagnosticsBox.setColour (juce::TextEditor::outlineColourId,    juce::Colour (0x1c000000));
        diagnosticsBox.setColour (juce::TextEditor::shadowColourId,     juce::Colour (0x16000000));

        cpuUsageLabel.setText ("CPU Usage", juce::dontSendNotification);
        cpuUsageText.setJustificationType (juce::Justification::right);
        addAndMakeVisible (&cpuUsageLabel);
        addAndMakeVisible (&cpuUsageText);

        setSize (760, 360);

        setAudioChannels (2, 2);
        deviceManager.addChangeListener (this);

        startTimer (50);
    }

    ~MainContentComponent() override
    {
        deviceManager.removeChangeListener (this);
        shutdownAudio();
    }

    void prepareToPlay (int, double) override {}

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto* device = deviceManager.getCurrentAudioDevice();

        auto activeInputChannels  = device->getActiveInputChannels();
        auto activeOutputChannels = device->getActiveOutputChannels();

        auto maxInputChannels  = activeInputChannels.countNumberOfSetBits();
        auto maxOutputChannels = activeOutputChannels.countNumberOfSetBits();

        for (auto channel = 0; channel < maxOutputChannels; ++channel)
        {
            if ((! activeOutputChannels[channel]) || maxInputChannels == 0)
            {
                bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                auto actualInputChannel = channel % maxInputChannels;

                if (! activeInputChannels[channel])
                {
                    bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
                }
                else
                {
                    auto* inBuffer  = bufferToFill.buffer->getReadPointer  (actualInputChannel,
                                                                            bufferToFill.startSample);
                    auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);

                    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                        outBuffer[sample] = inBuffer[sample] * random.nextFloat() * 0.25f;
                }
            }
        }
    }

    void releaseResources() override {}

    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::grey);
        g.fillRect (getLocalBounds().removeFromRight (proportionOfWidth (0.4f)));
    }

    void resized() override
    {
        auto rect = getLocalBounds();

        audioSetupComp.setBounds (rect.removeFromLeft (proportionOfWidth (0.6f)));
        rect.reduce (10, 10);

        auto topLine (rect.removeFromTop (20));
        cpuUsageLabel.setBounds (topLine.removeFromLeft (topLine.getWidth() / 2));
        cpuUsageText .setBounds (topLine);
        rect.removeFromTop (20);

        diagnosticsBox.setBounds (rect);
    }

private:
    void changeListenerCallback (juce::ChangeBroadcaster*) override
    {
        dumpDeviceInfo();
    }

    static juce::String getListOfActiveBits (const juce::BigInteger& b)
    {
        juce::StringArray bits;

        for (auto i = 0; i <= b.getHighestBit(); ++i)
            if (b[i])
                bits.add (juce::String (i));

        return bits.joinIntoString (", ");
    }

    void timerCallback() override
    {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText (juce::String (cpu, 6) + " %", juce::dontSendNotification);
    }

    void dumpDeviceInfo()
    {
        logMessage ("--------------------------------------");
        logMessage ("Current audio device type: " + (deviceManager.getCurrentDeviceTypeObject() != nullptr
                                                     ? deviceManager.getCurrentDeviceTypeObject()->getTypeName()
                                                     : "<none>"));

        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            logMessage ("Current audio device: "   + device->getName().quoted());
            logMessage ("Sample rate: "    + juce::String (device->getCurrentSampleRate()) + " Hz");
            logMessage ("Block size: "     + juce::String (device->getCurrentBufferSizeSamples()) + " samples");
            logMessage ("Bit depth: "      + juce::String (device->getCurrentBitDepth()));
            logMessage ("Input channel names: "    + device->getInputChannelNames().joinIntoString (", "));
            logMessage ("Active input channels: "  + getListOfActiveBits (device->getActiveInputChannels()));
            logMessage ("Output channel names: "   + device->getOutputChannelNames().joinIntoString (", "));
            logMessage ("Active output channels: " + getListOfActiveBits (device->getActiveOutputChannels()));
        }
        else
        {
            logMessage ("No audio device open");
        }
    }

    void logMessage (const juce::String& m)
    {
        diagnosticsBox.moveCaretToEnd();
        diagnosticsBox.insertTextAtCaret (m + juce::newLine);
    }

    //==========================================================================
    juce::Random random;
    juce::AudioDeviceSelectorComponent audioSetupComp;
    juce::Label cpuUsageLabel;
    juce::Label cpuUsageText;
    juce::TextEditor diagnosticsBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
