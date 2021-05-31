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

#include <JuceHeader.h>
#include "MixdownFolder.h"

/**
* Constructor inherits from Audio device manager and state enumeration
* @see MixdownFolder.h
*/

MixdownFolderComp::MixdownFolderComp(juce::AudioDeviceManager& adm, LayerRecorderComponent& layerRecorder) : deviceManager(adm), state(Stopped), layerRecorder(layerRecorder) {
    addAndMakeVisible(zoomSlider);
    zoomSlider.setRange(0, 1, 0);
    zoomSlider.onValueChange = [this] { tn->setZoomFactor(zoomSlider.getValue()); };
    zoomSlider.setSkewFactor(2);

    tn.reset(new ThumbnailComp(audioFormatManager, transport, zoomSlider));
    std::cout << tn->getName() << std::endl;
    addAndMakeVisible(tn.get());
    tn->addChangeListener(this);

    thread.startThread(3);
    
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

    addAndMakeVisible(&audioPositionSlider);
    audioPositionSlider.setRange(0, 0.1);
    audioPositionSlider.setTextValueSuffix(" Sec");
    audioPositionSlider.setNumDecimalPlacesToDisplay(3);
    //Sets the entire component with a slider listener
    audioPositionSlider.addListener(this);
    //Slider's label
    addAndMakeVisible(&sliderLabel);
    sliderLabel.setText("Position", juce::dontSendNotification);
    sliderLabel.attachToComponent(&audioPositionSlider, true);

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
* Destructor
* @see MixdownFolder.h
*/
MixdownFolderComp::~MixdownFolderComp() 
{
    transport.setSource(nullptr);
    tn->removeChangeListener(this);
}

/*
  ==============================================================================

    AUDIO METHODS

  ==============================================================================
*/

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    //Clear buffer region to fill in new buffer with audio chunks
    if (reader.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    transport.getNextAudioBlock(bufferToFill);
    //audioPositionSlider.setNumDecimalPlacesToDisplay(3);
    //audioPositionSlider.setValue(round(transport.getCurrentPosition() * 10000.0)/10000.0);
}

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::releaseResources() { transport.releaseResources(); }

/*
  ==============================================================================

    CHANGE LISTENER CALLBACK METHODS
  ==============================================================================
*/

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == tn.get()) {
        showAudioResource(URL(tn->getLastDroppedFile()));
    }

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
* @see MixdownFolder.h
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
                audioPositionSlider.setValue(0);
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
* @see MixdownFolder.h
*/
void MixdownFolderComp::sliderValueChanged(juce::Slider* slider) {
    if (slider == &audioPositionSlider) {
        transport.setPosition(audioPositionSlider.getValue());
    }
}

/**
* @see MixdownFolder.h
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

        //audioPositionSlider.setValue(0);
        //audioPositionSlider.setRange(0, (fileReader->lengthInSamples / fileReader->sampleRate));

        juce::URL* url = new juce::URL(selected.getMixdownFile());
        juce::URL& url2 = *url;
        tn->setURL(url2);

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

/*
  ==============================================================================

    BUTTON CLICK RESPONSES

  ==============================================================================
*/

/**
* @see MixdownFolder.h
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
* @see MixdownFolder.h
*/
void MixdownFolderComp::nextButtonClickResponse() {
    TransportState lastState = state;
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    //Set unique ids for indices inside fileBoxMenu
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
* @see MixdownFolder.h
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
* @see MixdownFolder.h
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
* @see MixdownFolder.h
*/
void MixdownFolderComp::stopButtonClickResponse() {
    if (state == Paused) {
        stateChange(Stopped);
    } else {
        stateChange(Stopping);
    }
}

/*
  ==============================================================================

    REDRAW METHODS

  ==============================================================================
*/

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::paint(juce::Graphics& g) {
    // backgroundColourId is an enumeration for default grey background color.
    g.fillAll (juce::Component::getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::resized() {
    
    auto area = getLocalBounds();
    
    // playback thumbnail and slider
    auto r = area.removeFromTop(165);
    tn->setBounds(r.removeFromTop(140));
    auto zoom = r.removeFromTop(25);
    zoomSlider.setBounds(zoom);
    
    //Scaled to the parent window view
    fileButton.setBounds(area.removeFromTop(41).reduced(8));
    fileBoxMenu.setBounds(area.removeFromTop(41).reduced(8));
    
    // transport buttons
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
    
    prevNextGrid.performLayout(area.removeFromTop(80).reduced(8));
    
}


/*
  ==============================================================================

    HELPER METHODS

  ==============================================================================
*/


/*
* Function rounds a given number in the form val = (val * 10^decimals)
* Example : val * 10000.0 means that we round the fourths place
* 
* @param val  The double value that is being rounded
* @return double  A value that has been rounded up or down to nearest decimals place.
*/
inline double round(double val)
{
    if (val < 0) return ceil(val - 0.5);
    return floor(val + 0.5);
}
