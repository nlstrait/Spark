# Status Report #3 - April 28th

## Team Report


### Plans and goals from last week

*   Agenda for Thursday project meeting
    *   Review Architecture and Design assignment and decide approach
*   Prototype with ultimate intention of deciding on a toolset
*   Learn basics of the framework(s) we will use
*   GUI mockups and general GUI concept/design

### Progress and issues

*   Successfully build and ran JUCE examples
*   Gained some familiarity with basic JUCE concepts


### Plans and goals for following week

*   Agenda for Thursday project meeting
    *   Discuss new Tuesday meeting approach
        *   End meetings with discussion about next week's assignment (now that Rene will be releasing all assignments up front) so that our Wednesday progress reports (and plans within) can fully address the upcoming assignment
    *  Look over Testing and Continuous Integration assignment together and decide what, if anything, needs to be done before the start of next calendar week
*   Continue getting to know JUCE
*   Decide on and setup...
    *   Test-automation infrastructure
    *   CI service



## Contributions


### Nolan Strait

*   Plans and goals from last week
    *   Actually get to doing what I said I wanted to last week: use JUCE to interface with an audio input device and record.
    *   Become more familiar with JUCE's architecture and design 
    *   Effectively add functionality to a JUCE example
*   Progress and issues
    *   Ran into issue with using JUCE to interface with audio input devices on macOS
    *   Learned more about JUCE's architecture
    *   Layed out rough sketch of Spark's architecture using JUCE as inspiration
        *   It is difficult if not impossible to decouple JUCE's architecture from Spark's
*   Plans and goals for following week
    *   Determine root cause of audio input device issue w/ JUCE on macOS
        *   Perhaps only on my machine, just happens to be macOS
        *   Confirm that identical examples are working properly for teammates
    *   Continue familiarizing myself with JUCE
    *   Iterate on ideas for approaches to testing our code
        *   Distinguish independent components from those which will be largely tied to JUCE
        *   Research JUCE's testing system for the sake of testing JUCE-dependent components


### Mark Lei

*   Plans and goals from last week
    *   test out Juce and SoundDevice and decide which's better
    *   Become familiar with both options
*   Progress and issues
    *   Not familiar with Tkinkter, so we will most likely be using Juce and C++.
    *   Rusty with C++, will probably need some time to relearn it.
*   Plans and goals for following week
    *   Get familiar with C++ as well as Juce.
    *   Create buttons related to project in Juce by this week
    *   Start to write codes.


### Jay Grinols

*   Plans and goals from last week
    *  Since our python library choices are a little all over the place, I want to try and work my way around using them and see if it's worth using them instead of JUCE.
*   Progress and issues
    *   Popsicle doesn't seem to work well for windows
    *   Mac OS popsicle was easy to set up, but it wasn't well documented
    *   JUCE in C++ is probably the best option for us because of overall compatibility
*   Plans and goals for following week
    *   Start learning JUCE framework
    *   Look at tutorials to see if anything similar exists already for playback


### Tuan Thai

*   Plans and goals from last week
    *   Make a decision between JUCE and our combination of Python frameworks
    *   Create GUI mockups using TKinter to compare to JUCE examples
    *   Attempt to get popsicle to work
    *   Start learning about making folder system to connect local folder system to app
*   Progress and issues
    *   We ended up scrapping popsicle to go for C++
    *   Tkinter is usable but very rough around the edges
    *   Python SoundDevice and PyDub works but not fully complete
*   Plans and goals for following week
    *   Brush up on C++ and get used to JUCE
    *   Read through more JUCE tutorials for GUI
    *   Learn JUCE integration with MVC
    *   Write code for my project portion
