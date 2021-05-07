# Spark: quick-capture for music producers

## Overview
Creators shouldn't have to open an entire heavy-weight DAW (Digital Audio Workstation) to record new material for existing projects. Spark aims to eliminate friction in the music production process by enabling creators to easily browse through mixdowns and quickly capture inspired additions.

### Terminology
**Mixdown:** an audio file which is a render of some audio project. [Read here](https://backtracks.fm/resources/podcast-dictionary/mixdown+of+audio) for a more in-depth definition.

## Repository Structure
We are currently in an experimental phase, so our repository structure has yet to settle. It will ultimately dependent on our tool(s)/framework(s) of choice. 
### `prototypes/`
Here you can find current prototypes. We are using this space to experiment with and decide between two primary toolsets:
1. [JUCE](https://juce.com/) (C++ framework for audio application development, GUI functionality included)
2. [Tkinter](https://wiki.python.org/moin/TkInter) (Python GUI toolkit) and [python-sounddevice](https://python-sounddevice.readthedocs.io/en/latest/index.html) (Python audio processing library)

## Building and Developing with JUCE
Some of our prototypes are built with JUCE. Since JUCE is a heavy framework, building and running these prototypes requires first setting up and getting a little familiar with JUCE and its project manager, Projucer. Follow [this JUCE guide](https://docs.juce.com/master/tutorial_new_projucer_project.html) to do so. Once JUCE is installed, you can open a `*.jucer` file using Projucer to setup a project and open it in an IDE of your choice. 

### macOS
In order for JUCE to access audio input devices, it needs to register and receive permission. To run and debug with audio input, enable Microphone Access for your specific project and exporter within the Projucer.

### Other quirks of JUCE development
As stated in [this JUCE tutorial](https://docs.juce.com/master/tutorial_main_component.html), you should "always use The Projucer to create new files; never do so from your IDE (The Projucer would overwrite such changes the next time you save your project)."