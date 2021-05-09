# Spark: quick-capture for music producers

## Overview
Creators shouldn't have to open an entire heavy-weight DAW (Digital Audio Workstation) to record new material for existing projects. Spark aims to eliminate friction in the music production process by enabling creators to easily browse through mixdowns and quickly capture inspired additions.

### Terminology
**Mixdown:** an audio file which is a render of some audio project. [Read here](https://backtracks.fm/resources/podcast-dictionary/mixdown+of+audio) for a more in-depth definition.

## Repository Structure
### `.prototypes/`
Here we have early experiments with JUCE.

### `Source/`
Top-level directory for Spark's source code.

### `Spark.jucer`
JUCE project configuration. Necessary for managing Spark through the Projucer.

### `test/`
Here we have our [GoogleTest](https://github.com/google/googletest) testing suite.

## Building and Developing with JUCE
Spark is largely a [JUCE](https://juce.com/)-based project. Since JUCE is such a heavy framework, building and running Spark (as well as our prototypes) requires first setting up and getting a little familiar with JUCE and its project manager, Projucer. Follow [this JUCE guide](https://docs.juce.com/master/tutorial_new_projucer_project.html) to do so. Once JUCE is installed, you can open a `*.jucer` file using the Projucer to setup a project and open it in an IDE of your choice. 

### macOS
In order for JUCE to access audio input devices, it needs to register and receive permission. To run and debug with audio input, enable Microphone Access for your specific project and exporter within the Projucer.

### Other quirks of JUCE development
As stated in [this JUCE tutorial](https://docs.juce.com/master/tutorial_main_component.html), you should "always use The Projucer to create new files; never do so from your IDE (The Projucer would overwrite such changes the next time you save your project)."