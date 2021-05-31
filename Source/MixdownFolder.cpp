/*
  ==============================================================================

    MixdownFolder.cpp
    Created: 11 May 2021
    Author: Tuan Thai

    MixdownFolderComp represents a component that encompasses the mixdown folder
    file querying system and dropdown population. It also includes the mixdown folder
    selection via next/previous and click selection. The last feature that it
    includes is the playback feature through play/stop.
  ==============================================================================
*/

#include "MixdownFolder.h"

/**
* Acts as constructor to set up necessary variables and functions.
*/
MixdownFolderComp::MixdownFolderComp(juce::AudioDeviceManager& adm, LayerRecorderComponent& layerRecorder) : deviceManager(adm), state(Stopped), layerRecorder(layerRecorder) {
    
    addAndMakeVisible(&fileBoxMenu);
    fileBoxMenu.setJustificationType(juce::Justification::centred);
    //Lambda captures event on bar change and calls function
    fileBoxMenu.onChange = [this] {fileBoxMenuChanged(); };

    addAndMakeVisible(&fileButton);
    fileButton.setButtonText("Select folder");
    //Lambda captures event on button click and calls function
    fileButton.onClick = [this] {fileButtonClickResponse(); };

    addAndMakeVisible(&nextButton);
    nextButton.setButtonText("Next");
    nextButton.setEnabled(false);
    //Lambda captures event on button click and calls function
    nextButton.onClick = [this] {nextButtonClickResponse(); };

    addAndMakeVisible(&prevButton);
    prevButton.setButtonText("Prev");
    prevButton.setEnabled(false);
    //Lambda captures event on button click and calls function
    prevButton.onClick = [this] {prevButtonClickResponse(); };

    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    //Lambda captures event on button click and calls function
    playButton.onClick = [this] {playButtonClickResponse(); };
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    //Lambda captures event on button click and calls function
    stopButton.onClick = [this] {
        stopButtonClickResponse();
        this->layerRecorder.stopRecording();
    };
    stopButton.setEnabled(false);

    //Registers the basic format of WAV and RIFF files
    audioFormatManager.registerBasicFormats();
    //Listener for transport source changes
    transport.addChangeListener (this);

}

/**
* Mixdown folder destructor.
*/
MixdownFolderComp::~MixdownFolderComp() {}

/**
* Function sets up the transport source.
* 
* @param samplesPerBlockExpected  Number of samples per block to be loaded in.
* @param sampleRate  Expected sample rate of files.
*/
void MixdownFolderComp::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/*
* Function gets the next audio block of the current audio file.
* 
* @param bufferToFill  A buffer that holds audio blocks from a selected audio file, in form
*   AudioSourceChannelInfo object.
*/
void MixdownFolderComp::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (reader.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    transport.getNextAudioBlock(bufferToFill);
}

/**
* Function releases resources when they are no longer needed.
*/
void MixdownFolderComp::releaseResources() { transport.releaseResources(); }

/**
* Callback function that projects changes audio transport source.
* Changes come in the form of state changes.
* 
* @param source  Changebroadcaster that sends messages to related classes.
*/
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

void MixdownFolderComp::triggerPlayback() {
    if (state != Playing)
        playButton.triggerClick();
}

/**
* Function updates dropdown bar with selected audio file and primes playback.
* Component state is reverted to state.Stopped if it has been changed.
* Called by "Select folder" button onChange event listener.
*/
void MixdownFolderComp::fileBoxMenuChanged() {
    int fileID = fileBoxMenu.getSelectedId();
    if (fileID == 0) return;
    //fileBoxMenu indices starts at 1 but array indices start at 0
    Project& selected = projects.getReference(fileID-1);
    layerRecorder.setProject(&selected);
    if (layerRecorder.isRecording()) {
        // continue recording, but for new project
        layerRecorder.stopRecording();
        layerRecorder.startRecording();
    }
    
    juce::AudioFormatReader* fileReader = audioFormatManager.createReaderFor(selected.getMixdownFile());

    if (fileReader != nullptr) {
        std::unique_ptr< juce::AudioFormatReaderSource>
            tempReader(new juce::AudioFormatReaderSource(fileReader, true));

        //fileReader->sampleRate handles hardware file sample rate match up
        transport.setSource(tempReader.get(), 0, nullptr, fileReader->sampleRate);
        playButton.setEnabled(true);

        reader.reset(tempReader.release());
    }

    //Set bounds of next/prev buttons
    if (fileBoxMenu.getSelectedId() == projects.size()) {
        nextButton.setEnabled(false);
        prevButton.setEnabled(true);
    } else if (fileBoxMenu.getSelectedId() <= 1) {
        nextButton.setEnabled(true);
        prevButton.setEnabled(false);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
}

/**
* Function opens a directory to query user directory selection.
* Audio files are loaded into fileBoxMenu
*/
void MixdownFolderComp::fileButtonClickResponse() {
    juce::FileChooser fileChooser("Choose a mixdown folder");
    //Awaits user directory selection
    if (fileChooser.browseForDirectory()) {
        juce::File myDirectory;
        myDirectory = fileChooser.getResult();
        fileBoxMenu.clear();
        projects = ProjectManagement::getAllProjectsInFolder(myDirectory);

        for (int i = 0; i < projects.size(); i++) {
            //fileBoxMenu indices starts at 1 but array indices start at 0
            fileBoxMenu.addItem(projects[i].getName(), i + 1);
        }
        if (projects.size() > 0) {
            nextButton.setEnabled(true);
            // load up the first project
            nextButton.triggerClick();
        }
    }
}

/**
* Function selects the next track in the loaded mixdown folder.
* Component state is reverted to state.Stopped if it has been changed.
* Called by "Next" button onClick event listener.
*/
void MixdownFolderComp::nextButtonClickResponse() {
    TransportState lastState = state;
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    fileBoxMenu.setSelectedId(currentID + 1);

    //Last possible choice is the final track in the folder.
    if (fileBoxMenu.getSelectedId() == projects.size()) {
        nextButton.setEnabled(false);
        prevButton.setEnabled(true);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
    
    // Continue playing (begin playing new track seemlessly)
    if (lastState == Playing)
        playButton.triggerClick();
}

/**
* Function selects the previous track in the loaded mixdown folder.
* Component state is reverted to state.Stopped if it has been changed.
* Called by "Prev" button onClick event listener.
*/
void MixdownFolderComp::prevButtonClickResponse() {
    TransportState lastState = state;
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    fileBoxMenu.setSelectedId(currentID - 1);

    //First possible choice is no track.
    if (fileBoxMenu.getSelectedId() <= 1) {
        nextButton.setEnabled(true);
        prevButton.setEnabled(false);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
    
    if (lastState == Playing) {
        // Continue playing (begin playing new track seemlessly)
        playButton.triggerClick();
    }
}

/**
* Function changes component state to state.Starting or state.Pausing.
* Called by "Play"/"Resume" button onClick event listener.
* "Resume" starts the playback from the last stop.
* "Play" starts the playback.
*/
void MixdownFolderComp::playButtonClickResponse() {
    if ((state == Stopped) || (state == Paused)) {
        stateChange(Starting);
    }
    else if (state == Playing) {
        stateChange(Pausing);
    }
}

/**
* Function changes component state to state.Stopped or state.Stopping.
* Called by "Stop"/"Reset" button onClick event listener.
* "Reset" restarts the playback from the start.
* "Stop" stops the playback.
*/
void MixdownFolderComp::stopButtonClickResponse() {
    if (state == Paused) {
        stateChange(Stopped);
    } else {
        stateChange(Stopping);
    }
}

/**
* Function redraws parts of component that require updates.
* Called when a part of the MixDownFolder component requires redrawing.
* 
* @param g  The graphics context passed to this class for any drawing.
*/
void MixdownFolderComp::paint(juce::Graphics& g) {
    // backgroundColourId is an enumeration for default grey background color.
    g.fillAll (juce::Component::getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}


/**
* Function resizes the dimensions of component dynamically to parent size.
* Called everytime the MixDownFolder component size is changed. 
*/
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
