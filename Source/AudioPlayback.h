/*
  ==============================================================================

    AudioPlayback.h
    Created: 10 May 2021 2:03:45pm
    Author:  Nolan Strait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DemoThumbnailComponent :  public juce::Component,
                                public juce::ChangeListener,
                                public juce::FileDragAndDropTarget,
                                public juce::ChangeBroadcaster,
                                private juce::ScrollBar::Listener,
                                private juce::Timer
{
public:
    DemoThumbnailComponent(juce::AudioFormatManager& formatManager
                           // juce::AudioTransportSource& source,
                           //juce::Slider& slider
    );
    ~DemoThumbnailComponent() override;
    
    void setURL(const juce::URL& url);
    
    juce::URL getLastDroppedFile() const noexcept { return lastFileDropped; }
    
    void setZoomFactor(double amount);
    
    void setRange (juce::Range<double> newRange);
    
    void setFollowsTransport(bool shouldFollow) { isFollowingTransport = shouldFollow; }
    
    void paint(juce::Graphics& g) override;
    
    void resized() override;
    
    // Called by the thumbnail when it has changed
    void changeListenerCallback(juce::ChangeBroadcaster*) override { repaint(); }
    
    bool isInterestedInFileDrag(const juce::StringArray& /*files*/) override { return true; }
    
    void filesDropped(const juce::StringArray& files, int /*x*/, int /*y*/) override;
    
    void mouseDown(const juce::MouseEvent& e) override { mouseDrag(e); }
    
    void mouseDrag(const juce::MouseEvent& e) override;
    
    void mouseUp(const juce::MouseEvent&) override { /* transportSource.start(); */ }
    
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;

    
private:
    // juce::AudioTransportSource& transportSource;
    // juce::Slider& zoomSlider;
    juce::ScrollBar scrollbar { false };
    
    juce::AudioThumbnailCache thumbnailCache { 5 };
    juce::AudioThumbnail thumbnail;
    juce::Range<double> visibleRange;
    bool isFollowingTransport = false;
    
    juce::URL lastFileDropped;
    
    juce::DrawableRectangle currentPositionMarker;
    
    
    float timeToX (const double time) const;
    
    double xToTime(const float x) const;
    
    bool canMoveTransport() const noexcept;
    
    void scrollBarMoved(juce::ScrollBar* scrollBarThatHasMoved, double newRangeStart) override;
    
    void timerCallback() override;
    
    void updateCursorPosition();
};
