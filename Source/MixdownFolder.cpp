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
MixdownFolderComp::MixdownFolderComp(juce::AudioDeviceManager& adm) : deviceManager(adm), state(Stopped) {
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
    nextButton.setEnabled(true);
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
    stopButton.onClick = [this] {stopButtonClickResponse(); };
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
    //fileBoxMenu indices starts at 1 but array indices start at 0
    selectedAudioFile = audioFiles[fileID-1]; 
    
    juce::AudioFormatReader* fileReader = audioFormatManager.createReaderFor(selectedAudioFile);

    if (fileReader != nullptr) {
        std::unique_ptr< juce::AudioFormatReaderSource>
            tempReader(new juce::AudioFormatReaderSource(fileReader, true));

        //fileReader->sampleRate handles hardware file sample rate match up
        transport.setSource(tempReader.get(), 0, nullptr, fileReader->sampleRate);

        //audioPositionSlider.setValue(0);
        //audioPositionSlider.setRange(0, (fileReader->lengthInSamples / fileReader->sampleRate));

        juce::URL* url = new juce::URL(selectedAudioFile);
        juce::URL& url2 = *url;
        tn->setURL(url2);

        playButton.setEnabled(true);

        reader.reset(tempReader.release());
    }

    if (state != Stopped) {
        stateChange(Stopped);
    }

    //Set bounds of next/prev buttons
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

/*
  ==============================================================================

    BUTTON CLICK RESPONSES

  ==============================================================================
*/

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::fileButtonClickResponse() {
    //"userMusicDirectory" redirects to windows default music directory
    juce::FileChooser fileChooser("Choose a mixdown folder",
        juce::File::getSpecialLocation(juce::File::userMusicDirectory));

    //Awaits user directory selection
    if (fileChooser.browseForDirectory()) {
        juce::File myDirectory;
        myDirectory = fileChooser.getResult();

        //Only wav files are allowed
        myDirectory.findChildFiles(audioFiles, juce::File::findFiles, true, "*.wav");

        //Clear file box to reset indices
        fileBoxMenu.clear();
        for (int i = 0; i < audioFiles.size(); i++) {
            //fileBoxMenu indices starts at 1 but array indices start at 0
            fileBoxMenu.addItem(audioFiles[i].getFileName(), i + 1);
        }
    }
}

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::nextButtonClickResponse() {
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    //Set unique ids for indices inside fileBoxMenu
    fileBoxMenu.setSelectedId(currentID + 1);

    //Last possible choice is the final track in the folder.
    if (fileBoxMenu.getSelectedId() == audioFiles.size()) {
        nextButton.setEnabled(false);
        prevButton.setEnabled(true);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
    }
}

/**
* @see MixdownFolder.h
*/
void MixdownFolderComp::prevButtonClickResponse() {
    if (state != Stopped) {
        stateChange(Stopped);
    }

    int currentID = fileBoxMenu.getSelectedId();
    fileBoxMenu.setSelectedId(currentID - 1);

    //First possible choice is no track.
    if (fileBoxMenu.getSelectedId() == 0) {
        nextButton.setEnabled(true);
        prevButton.setEnabled(false);
    } else {
        nextButton.setEnabled(true);
        prevButton.setEnabled(true);
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
    
    //Scaled to the parent window view
    fileButton.setBounds(area.removeFromTop(31).reduced(8));
    fileBoxMenu.setBounds(area.removeFromTop(31).reduced(8));
    
    //juce::Grid prevNextGrid;
    //using Track = juce::Grid::TrackInfo;
    //using Fr = juce::Grid::Fr;
    

    auto r = getLocalBounds().reduced(4);
    auto controls = r.removeFromBottom(90);
    auto zoom = controls.removeFromTop(25);
    zoomSlider.setBounds(zoom);
    r.removeFromBottom(6);
    tn->setBounds(r.removeFromBottom(140));


    //audioPositionSlider.setBounds(area.removeFromTop(41).reduced(8));

    //prevNextGrid.templateRows = { Track (Fr (1)), Track (Fr (1)) };
    //prevNextGrid.templateColumns = { Track (Fr (1)), Track (Fr (1)) };
    
    //prevNextGrid.items = {
        //juce::GridItem(prevButton), juce::GridItem(nextButton),
        //juce::GridItem(stopButton), juce::GridItem(playButton)
    //};
    
    //prevNextGrid.setGap(juce::Grid::Px(12));
    
    //prevNextGrid.performLayout(area.removeFromTop(80).reduced(8));
    
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