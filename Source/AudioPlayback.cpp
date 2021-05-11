/*
  ==============================================================================

    AudioPlayback.cpp
    Created: 10 May 2021 2:53:24pm
    Author:  Nolan Strait

  ==============================================================================
*/

#include "AudioPlayback.h"

DemoThumbnailComponent::DemoThumbnailComponent(juce::AudioFormatManager& formatManager
                                               // juce::AudioTransportSource& source,
                                               //juce::Slider& slider
)
    :   // transportSource(source),
        // zoomSlider(slider),
        thumbnail(512, formatManager, thumbnailCache)
{
    thumbnail.addChangeListener(this);
    
    addAndMakeVisible(scrollbar);
    scrollbar.setRangeLimits(visibleRange);
    scrollbar.setAutoHide(false);
    scrollbar.addListener(this);
    
    currentPositionMarker.setFill(juce::Colours::white.withAlpha(0.85f));
    addAndMakeVisible(currentPositionMarker);
}

DemoThumbnailComponent::~DemoThumbnailComponent() {
    scrollbar.removeListener(this);
    thumbnail.removeChangeListener(this);
}

void DemoThumbnailComponent::setURL(const juce::URL &url) {
    juce::InputSource* inputSource = nullptr;
    
    #if ! JUCE_IOS
    if (url.isLocalFile()) {
        inputSource = new juce::FileInputSource (url.getLocalFile());
    } else
    #endif
    {
        if (inputSource == nullptr)
            inputSource = new juce::URLInputSource (url);
    }

    if (inputSource != nullptr) {
        thumbnail.setSource (inputSource);

        juce::Range<double> newRange (0.0, thumbnail.getTotalLength());
        scrollbar.setRangeLimits (newRange);
        setRange (newRange);

        startTimerHz (40);
    }
}

void DemoThumbnailComponent::setZoomFactor(double amount) {
    if (thumbnail.getTotalLength() > 0) {
        auto newScale = juce::jmax(0.001, thumbnail.getTotalLength() * (1.0 - juce::jlimit(0.00, 0.99, amount)));
        auto timeAtCentre = xToTime((float) getWidth() / 2.0f);
        
        setRange({ timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5 });
    }
}

void DemoThumbnailComponent::setRange(juce::Range<double> newRange) {
    visibleRange = newRange;
    scrollbar.setCurrentRange(visibleRange);
    updateCursorPosition();
    repaint();
}

void DemoThumbnailComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::lightblue);
    
    if (thumbnail.getTotalLength() > 0.0) {
        auto thumbArea = getLocalBounds();
        
        thumbArea.removeFromBottom(scrollbar.getHeight() + 4);
        thumbnail.drawChannels(g, thumbArea.reduced(2), visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
    } else {
        g.setFont(14.0f);
        g.drawFittedText("(No audio file selected)", getLocalBounds(), juce::Justification::centred, 2);
    }
}

void DemoThumbnailComponent::resized() {
    scrollbar.setBounds(getLocalBounds().removeFromBottom(14).reduced(2));
}

void DemoThumbnailComponent::filesDropped(const juce::StringArray &files, int, int) {
    lastFileDropped = juce::URL(juce::File(files[0])); // why does this work?
    sendChangeMessage();
}

void DemoThumbnailComponent::mouseDrag(const juce::MouseEvent &e) {
    if (canMoveTransport())
        //transportSource.setPosition(juce::jmax(0.0, xToTime((float) e.x)));
        ;
}

void DemoThumbnailComponent::mouseWheelMove(const juce::MouseEvent &, const juce::MouseWheelDetails &wheel) {
    if (thumbnail.getTotalLength() > 0.0) {
        auto newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
        newStart = juce::jlimit(0.0, juce::jmax(0.0, thumbnail.getTotalLength() - (visibleRange.getLength())), newStart);
        
        if (canMoveTransport())
            setRange({ newStart, newStart + visibleRange.getLength() });
        /*
        if (wheel.deltaY != 0.0f)
            zoomSlider.setValue(zoomSlider.getValue() - wheel.deltaY);
        */
        repaint();
    }
}

float DemoThumbnailComponent::timeToX(const double time) const {
    if (visibleRange.getLength() <= 0)
        return 0;

    return (float) getWidth() * (float) ((time - visibleRange.getStart()) / visibleRange.getLength());
}

double DemoThumbnailComponent::xToTime(const float x) const {
    return (x / (float) getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
}

bool DemoThumbnailComponent::canMoveTransport() const noexcept {
    return ! (isFollowingTransport /* && transportSource.isPlaying() */ );
}

void DemoThumbnailComponent::scrollBarMoved(juce::ScrollBar *scrollBarThatHasMoved, double newRangeStart) {
    if (scrollBarThatHasMoved == &scrollbar)
        if (! (isFollowingTransport /* && transportSource.isPlaying() */ ))
            setRange (visibleRange.movedToStartAt (newRangeStart));
}

void DemoThumbnailComponent::timerCallback() {
    if (canMoveTransport())
        updateCursorPosition();
    else {
        //setRange (visibleRange.movedToStartAt (transportSource.getCurrentPosition() - (visibleRange.getLength() / 2.0)));
        setRange(visibleRange.movedToStartAt(0));
    }
}

void DemoThumbnailComponent::updateCursorPosition() {
    currentPositionMarker.setVisible (/* transportSource.isPlaying() || */ isMouseButtonDown());

    currentPositionMarker.setRectangle (juce::Rectangle<float> (timeToX ( /* transportSource.getCurrentPosition()*/ 0 ) - 0.75f, 0,
                                                          1.5f, (float) (getHeight() - scrollbar.getHeight())));
}
