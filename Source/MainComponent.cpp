#include "MainComponent.h"

//==============================================================================

/**
* Constructor
* @see MainComponent.h
*/
MainComponent::MainComponent()
    :   audioSetupComp (deviceManager,
                  0,     // minimum input channels
                  256,   // maximum input channels
                  0,     // minimum output channels
                  256,   // maximum output channels
                  false, // ability to select midi inputs
                  false, // ability to select midi output devi
                  false, // treat channels as stereo pairs
                  true),// hide advanced options
        layerRecorderComp(deviceManager),
        mixdownFolderComp(deviceManager, layerRecorderComp)
    {
        
    addAndMakeVisible(layerRecorderComp);
    // This is certainly bad practice, but alternatives require radical redesign
    layerRecorderComp.setTransport(mixdownFolderComp.getTransportPtr());
    layerRecorderComp.setPlaybackComp(&mixdownFolderComp);
    addAndMakeVisible(audioSetupComp);
        
    addAndMakeVisible(mixdownFolderComp);
    //mixdownFolderComp.addMouseListener(this, true); // not sure if necessary...
        
    setSize (1000, 500);
            
    //addAndMakeVisible(diagnosticsBox);
    diagnosticsBox.setMultiLine (true);
    diagnosticsBox.setReturnKeyStartsNewLine (true);
    diagnosticsBox.setReadOnly (true);
    diagnosticsBox.setScrollbarsShown (true);
    diagnosticsBox.setCaretVisible (false);
    diagnosticsBox.setPopupMenuEnabled (true);
    
    setAudioChannels(2, 2); // This could be more sophisticated to handle unconvential input/output setups
    deviceManager.addChangeListener(this);
}

/**
* Destructor
* @see MainComponent.h
*/
MainComponent::~MainComponent() {
    deviceManager.removeChangeListener (this);
    shutdownAudio();
}

//==============================================================================
/**
* Function redraws parts of component that require updates.
* Called when a part of the MixDownFolder component requires redrawing.
*
* @param g  The graphics context passed to this class for any drawing.
*/
void MainComponent::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
}

/**
* Function resizes the dimensions of component dynamically to parent size.
* Called everytime the MixDownFolder component size is changed.
*/
void MainComponent::resized() {
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    // Layout via Grid
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    
    grid.templateRows    = { Track (Fr (2)), Track (Fr (1)) };
    grid.templateColumns = { Track (Fr (4)), Track (Fr (3)) };
    
    grid.items = {
        juce::GridItem(mixdownFolderComp), juce::GridItem(audioSetupComp),
        juce::GridItem(layerRecorderComp)
    };
    
    auto rect = getLocalBounds();
    grid.performLayout(rect);
    
    // Layout by subdividing rectangles
     /*
    auto area = getLocalBounds();
    
    auto recorderWidth = 350;
    recorderComponent.setBounds(area.removeFromLeft(recorderWidth));
    
    auto audioSetupWidth = 350;
    audioSetupComp.setBounds(area.removeFromLeft(audioSetupWidth));
    diagnosticsBox.setBounds(area);
    */
}

/**
* Function sets up the transport source.
*
* @param samplesPerBlockExpected  Number of samples per block to be loaded in.
* @param sampleRate  Expected sample rate of files.
*/
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    mixdownFolderComp.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/**
* Function gets the next audio block of the current audio file.
*
* @param bufferToFill  A buffer that holds audio blocks from a selected audio file, in form
*   AudioSourceChannelInfo object.
*/
void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    //bufferToFill.clearActiveBufferRegion(); // prevent feedback
    mixdownFolderComp.getNextAudioBlock(bufferToFill);
}

/**
* Function logs current adapted audio device information to "diagnosticsBox" text box.
*/
void MainComponent::dumpDeviceInfo() {
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
    }
    else
    {
        logMessage ("No audio device open");
    }
}

/**
* Logs audio device diagnostics in form "^Diagnostics information here"
* @param m  A string to be converted to log message
*/
void MainComponent::logMessage(const juce::String &m) {
    diagnosticsBox.moveCaretToEnd();
    diagnosticsBox.insertTextAtCaret (m + juce::newLine);
}
