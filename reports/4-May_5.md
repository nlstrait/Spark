# Status Report #4 - May 5th

## Team Report


### Plans and goals from last week

*   Agenda for Thursday project meeting
    *   Discuss new Tuesday meeting approach
        *   End meetings with discussion about next week's assignment (now that Rene will be releasing all assignments up front) so that our Wednesday progress reports (and plans within) can fully address the upcoming assignment
    *  Look over Testing and Continuous Integration assignment together and decide what, if anything, needs to be done before the start of next calendar week
*   Continue getting to know JUCE
*   Decide on and setup...
    *   Test-automation infrastructure
    *   CI service



### Progress and issues

*   Effectively managing audio input/output devices and recording audio with JUCE on macOS
*   Researched multiple CI services and decided on GitHub Actions
    *   Created starter workflows for building and testing
*   Researched testing tools and (mostly) decided on GoogleTest
    *   Pending resolution; Jay voiced concern


### Plans and goals for following week

*   Agenda for Thursday project meeting
    *   Discuss new Tuesday meeting approach
        *   End meetings with discussion about next week's assignment (now that Rene will be releasing all assignments up front) so that our Wednesday progress reports (and plans within) can fully address the upcoming assignment
     *   Make final decision on test automation infrastructure: GoogleTest or not?
     *   Determine how to distribute remaining work
         *   Pair programming? Who will partner with who? What will they both be working on together? What will they work on separately? Who does it make the most sense to pair?
        *   Discuss continuous documentation and approaches
            *   One approach: add noteworthy documentation to root README with an intention of splitting up this README into separate files if necessary. There's no fear of over-documenting here, and we can all clearly see where we're at or when we split.
*   Review Beta Release assignment and expectations
    *   Solidify our toolchain, processes, and instructions
    *   Implement and integrate first versions of all of our system's major components
    *   Record a short demo and reflection



## Contributions


### Nolan Strait

*   Plans and goals from last week
    *   Determine root cause of audio input device issue w/ JUCE on macOS
        *   Perhaps only on my machine, just happens to be macOS
        *   Confirm that identical examples are working properly for teammates
    *   Continue familiarizing myself with JUCE
    *   Iterate on ideas for approaches to testing our code
        *   Distinguish independent components from those which will be largely tied to JUCE
        *   Research JUCE's testing system for the sake of testing JUCE-dependent components
*   Progress and issues
    *   Extended and enhanced documentation
    *   Determined root cause of audio input device issue on macOS
        *   macOS requires applications to request permission before accessing audio devices
        *   To fix, flag must be set using Projucer
            *   This has been noted in our root README
*   Plans and goals for following week
    *   Continue adding necessary documentation to root README as JUCE development continues
        *   Plan on splitting up this documentation into multiple files when/if necessary
        *   Encourage teammates to do the same
    *   Create JUCE-independent component(s) for project management
        *   Use Ben's feedback from Architecture and Design assignment to flesh out necessary details before attempting implementation
        *   Start with header files to create an API that becomes useful for other components
    *   Pair-program to develop and piece together components


### Mark Lei

*   Plans and goals from last week
    *   Planned to test JUCE and do some light testing/ coding
    *   Create prototypes
    *   Finish up re-learning C++
*   Progress and issues
    *   C++ is more complicated then what i learnt in high school, but good thing our project is not heavily relied on coding
    *   Finished a lot of JUCE tutorials and starting to make prototypes
    *   Had audio issue with JUCE for an hour or so, but fixed with software updates.
*   Plans and goals for following week
    *   Pair program this weekend to bring our prototypes together, see if anyone need helps.
    *   Create some tests after pair programming.


### Jay Grinols

*   Plans and goals from last week
    *   Start learning JUCE framework
    *   Look at tutorials to see if anything similar exists already for playback
*   Progress and issues
    *   Found demos and tutorials that aligned really well with the playback portion
    *   Still don't really understand the structure of this demo yet, a lot of implementation in the header file
    *   Got github actions to work with JUCE project for Mac OS and ubuntu
*   Plans and goals for following week
    *   Get github actions to work for windows
    *   Start writing real code and pair programming

### Tuan Thai

*   Plans and goals from last week
    *   Planned to write some code for JUCE mixdown folder
    *   Create some mockup GUIs
    *   Finish more tutorials for JUCE
*   Progress and issues
    *   Severely overestimated the JUCE mixdown folder workload
    *   Got used to C++ and did some C++ tutorials
    *   Set up CI and Build and some test suites
    *   Finished JUCE tutorials
*   Plans and goals for following week
    *   Finish up folder feature
    *   Create an actual GUI for folder
    *   Create a way to drag and drop files
    *   Perhaps create a basic save button
    *   Create more tests
