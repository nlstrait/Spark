/*
  ==============================================================================

    Model for the Spark application that ties in components through a parent view
    named mainWindow. Also ties in application functions and metadata.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
class SparkApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    SparkApplication() {}

    /**
    * Functions returns the prescribed project name
    * 
    * @return projectName  The string representation of the project name
    */
    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }

    /**
    * Functions returns the current application version
    *
    * @return versionString  The string representation of the project version
    */
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }

    /**
    * Functions sets the number of allowed instances (>= 1)
    *
    * @return bool  Value that allows more than one instance
    */
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // Initializes the spark application

        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {

        mainWindow = nullptr; //Allows for window shutdown
    }

    //==============================================================================
    /*
    * This function destroys current system processes and primes for shutdown.
    * Called whenever the system requests an application shutdown.
    */
    void systemRequestedQuit() override
    {
        quit();
    }

    /**
    * Called whenever another instance of this application is created.
    */
    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        
    }

    //==============================================================================
    /**
    * Main window is the parent view that Spark's components are built on top of.
    * It acts as a visual representation of our applications main window.
    * Called upon application run.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            //Uses the OS native title bar
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent, true);

            //Helps with resizing per device and OS
           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // This functions prompts the application to shutdown processes with quit signal.
            // Called whenever user attempts to close the application.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }


    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (SparkApplication)
