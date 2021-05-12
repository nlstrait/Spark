/*
  ==============================================================================

    MixdownFolder.cpp
    Created: 11 May 2021 3:23:11pm
    Author:

  ==============================================================================
*/

#include "MixdownFolder.h"

MixdownFolderComp::MixdownFolderComp(juce::AudioDeviceManager& adm) : deviceManager(adm), state(Stopped) {
    
    addAndMakeVisible(&fileBoxMenu);
    fileBoxMenu.setJustificationType(juce::Justification::centred);
    fileBoxMenu.onChange = [this] {fileBoxMenuChanged(); };

    addAndMakeVisible(&fileButton);
    fileButton.setButtonText("Select audio file");
    fileButton.onClick = [this] {fileButtonClickResponse(); };

    addAndMakeVisible(&nextButton);
    nextButton.setButtonText("Next");
    nextButton.setEnabled(true);
    nextButton.onClick = [this] {nextButtonClickResponse(); };

    addAndMakeVisible(&prevButton);
    prevButton.setButtonText("Prev");
    prevButton.setEnabled(false);
    prevButton.onClick = [this] {prevButtonClickResponse(); };

    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.onClick = [this] {playButtonClickResponse(); };
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    stopButton.onClick = [this] {stopButtonClickResponse(); };
    stopButton.setEnabled(false);

    audioFormatManager.registerBasicFormats();
    transport.addChangeListener (this);

}

MixdownFolderComp::~MixdownFolderComp() {}

void MixdownFolderComp::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MixdownFolderComp::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (reader.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    transport.getNextAudioBlock(bufferToFill);
}

void MixdownFolderComp::releaseResources() { transport.releaseResources(); }

void MixdownFolderComp::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &transport) {
        if (transport.isPlaying()) {
            stateChange(Playing);
        } else if ((state == Stopping) || (state == Playing)) {
            stateChange(Stopped);
        } else if (state == Pausing) {
            stateChange(Paused);
        }
    }
}

void MixdownFolderComp::stateChange(TransportState newState) {
    if (state != newState) {
        state = newState;

        switch (state) {
            case Stopped:
                playButton.setButtonText("Play");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(false);
                transport.setPosition(0.0);
                break;

            case Starting:
                transport.start();
                break;

            case Playing:
                playButton.setButtonText("Pause");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(true);
                break;

            case Pausing:
                transport.stop();
                break;

            case Paused:
                playButton.setButtonText("Resume");
                stopButton.setButtonText("Reset");
                break;

            case Stopping:
                transport.stop();
                break;
        }
    }
}

void MixdownFolderComp::fileBoxMenuChanged() {
    int fileID = fileBoxMenu.getSelectedId();
    selectedAudioFile = audioFiles[fileID-1]; //This is due to the fact that fileBoxMenu starts at 1 but arrays at 0
    
    juce::AudioFormatReader* fileReader = audioFormatManager.createReaderFor(selectedAudioFile);

    if (fileReader != nullptr) {
        std::unique_ptr< juce::AudioFormatReaderSource>
            tempReader(new juce::AudioFormatReaderSource(fileReader, true));

        transport.setSource(tempReader.get(), 0, nullptr, fileReader->sampleRate);
        playButton.setEnabled(true);

        reader.reset(tempReader.release());
    }

    if (state != Stopped) {
        stateChange(Stopped);
    }

    if (fileBoxMenu.getSelectedId() == audioFiles.size()) {
        nextButton.setEnabled(false);
        prevButton.setEnabled(true);
    } else if (fileBoxMenu.getSelectedId() == 0) {
        nextButton.setEnabled(true);
        prevButton.setEnabled(false);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
}

// Prompts user to choose directory
void MixdownFolderComp::fileButtonClickResponse() {
    juce::FileChooser fileChooser("Choose a mixdown folder",
        juce::File::getSpecialLocation(juce::File::userMusicDirectory));

    if (fileChooser.browseForDirectory()) {
        juce::File myDirectory;
        myDirectory = fileChooser.getResult();

        myDirectory.findChildFiles(audioFiles, juce::File::findFiles, true, "*.wav");

        for (int i = 0; i < audioFiles.size(); i++) {
            //Arrays start at 0 but fileBoxMenu has to start at 1 per documentation
            fileBoxMenu.addItem(audioFiles[i].getFileName(), i + 1);
        }
    }
}

void MixdownFolderComp::nextButtonClickResponse() {
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    fileBoxMenu.setSelectedId(currentID+1);

    if (fileBoxMenu.getSelectedId() == audioFiles.size()) {
        nextButton.setEnabled(false);
        prevButton.setEnabled(true);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
}

void MixdownFolderComp::prevButtonClickResponse() {
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    fileBoxMenu.setSelectedId(currentID - 1);

    if (fileBoxMenu.getSelectedId() == 0) {
        nextButton.setEnabled(true);
        prevButton.setEnabled(false);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
}

void MixdownFolderComp::playButtonClickResponse() {
    if ((state == Stopped) || (state == Paused)) {
        stateChange(Starting);
    }
    else if (state == Playing) {
        stateChange(Pausing);
    }
}

void MixdownFolderComp::stopButtonClickResponse() {
    if (state == Paused) {
        stateChange(Stopped);
    } else {
        stateChange(Stopping);
    }
}

// Utilize painting functions to fill backgrounds and draw
void MixdownFolderComp::paint(juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Component::getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

// This is called everytime the window is resized
void MixdownFolderComp::resized() {
    
    auto area = getLocalBounds();
    
    fileButton.setBounds(area.removeFromTop(41).reduced(8));
    fileBoxMenu.setBounds(area.removeFromTop(41).reduced(8));
    
    juce::Grid prevNextGrid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    
    prevNextGrid.templateRows = { Track (Fr (1)), Track (Fr (1)) };
    prevNextGrid.templateColumns = { Track (Fr (1)), Track (Fr (1)) };
    
    prevNextGrid.items = {
        juce::GridItem(prevButton), juce::GridItem(nextButton),
        juce::GridItem(stopButton), juce::GridItem(playButton)
    };
    
    prevNextGrid.setGap(juce::Grid::Px(12));
    
    prevNextGrid.performLayout(area.removeFromTop(76).reduced(8));
    
}
