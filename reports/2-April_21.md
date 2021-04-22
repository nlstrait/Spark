# Status Report #2 - April 21st

## Team Report


### Plans and goals from last week

*   Agenda for Thursday project meeting
    *   Is Popsicle viable?
    *   What basic code skeleton do we need before we can effectively split off and do our own work (and/or pair program) on features?
    *   What combination of Python frameworks and tools we will use if Popsicle fails.
*   Learn basics of the framework(s) we will use
*   GUI mockups and general GUI concept/design


### Progress and issues

*   JUCE and Popsicle: installing and running
    *   macOS: success
*   Popsicle's documentation is lacking; straight-up JUCE may be easier to use, despite being C++


### Plans and goals for following week

*   Agenda for Thursday project meeting
    *   Review Architecture and Design assignment and decide approach
*   Prototype with ultimate intention of deciding on a toolset
*   Learn basics of the framework(s) we will use
*   GUI mockups and general GUI concept/design



## Contributions


### Nolan Strait

*   Plans and goals from last week
    *   Learn basics of framework(s) for interfacing with audio input device(s) and recording audio input through creation of a prototype application that…
        *   takes an audio device and output directory/filename as input,
        *   begins recording upon launch (if audio device is found),
        *   and saves recording upon close.
*   Progress and issues
    *   Successfully installed Popsicle and JUCE on macOS
    *   Played with Popsicle examples and found issues
        *   Popsicle documentation is lacking
        *   One crucial example application would not run properly, and digging through the documentation and example code resulted in nothing
    *   Walked through and built JUCE examples
        *   Included simple audio file playback application
        *   Learned some of JUCE's architecture and design
*   Plans and goals for following week
    *   Actually get to doing what I said I wanted to last week: use JUCE to interface with an audio input device and record.
    *   Become more familiar with JUCE's architecture and design 
    *   Effectively add functionality to a JUCE example



### Mark Lei

*   Plans and goals from last week
    *   Tried to run Popsicle on windows with no success.
    *   Starts to construct the buttons, and what they do in the program
*   Progress and issues
    *   Popsicle is not working on Windows
    *   Started on Juce for the first time, trying out the features to see if it works okay on Windows.
*   Plans and goals for following week
    *   Get a prototype done on Juce and get a feeling on what it is capable of. If it's not efficient then maybe we will go back to Tkinter and Python.


### Jay Grinols

*   Plans and goals from last week
    *   Figuring out whether to rely on popsicle or a combination of other python libraries
    *   Try to get popsicle to work on windows/linux vm
    *   Start to prototype major features of the apps
    *   Find out how feasible the technologies we chose are for the task
    *   Figure out git workflow for the team
*   Progress and issues
    *   Popsicle documentation is pretty lacking, and it's extremely difficult to get working on a windows machine, didn't end up getting it working.
    *   Still haven't gotten a chance to see if the technologies we chose work for our purposes
*   Plans and goals for following week
*  Since our python library choices are a little all over the place, I want to try and work my way around using them and see if it's worth using them instead of JUCE.


### Tuan Thai

*   Plans and goals from last week
    *   Planning on trying to run Popsicle framework on windows machine.
    *   Looking into suitable ways to use JUCE or Python Combinations if Popsicle isn't working.
    *   Learning the basics of our frameworks and tools in order to design and create a mixdown folder.
    *   Should allow access to locale in order to select individual mixdowns.
    *   Start up with the last mixdown folder as the default.
*   Progress and issues
    *   Popsicle is hard to get running on a windows machine.
    *   Popsicle lacks documentation and thus makes it hard to understand the GUI examples.
    *   I assume that Popsicle and JUCE should be linked enough that the GUI examples we chose can follow.
    *   I got Popsicle to work on a linux machine but I don't always have access to said linux machine.
    *   Currently using JUCE to make some GUI mockups and looking into mixdown folders.
    *   May have to offset plans and goals from last week to test out prototypes first.
*   Plans and goals for following week
    *   Create some GUI mockups on JUCE and Python TKinter to compare.
    *   Utilize JUCE and C++ frameworks to create mixdown folder.
    *   Compare previous point to a version created purely from Python sound device and other tools.
    *   Hopefully get a working version of mixdown folder and some semblance of a GUI for it.
