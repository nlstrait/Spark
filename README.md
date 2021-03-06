# Spark: quick-capture for music producers

## Overview
Creators shouldn't have to open an entire heavy-weight DAW (Digital Audio Workstation) to record new material for existing projects. Spark aims to eliminate friction in the music production process by enabling creators to easily browse through mixdowns and quickly capture inspired additions.

[Final Release Demo & Reflection](https://youtu.be/rCKlBUKajjM)

## User Documentation
If you are not a developer and would just like to use Spark, please [see our User Documentation here](https://docs.google.com/document/d/1uvdGcDdwmmioiQLi-uWVZDDIGqzRN33mTG3bzXqQsGM/edit?usp=sharing).

## Project Documentation
For more in-depth project documentation, including team info, product description, use cases, requirements, team process, software architecture and design, [click here](https://docs.google.com/presentation/d/1OeaRLWhp0nDD3wk7fqoq211HNEdyreYjK5Wn-TSyaQs/edit?usp=sharing).

## Repository Structure
| File/Directory    | Description |
| --------------    | ----------- |
| `.prototypes/`    | Early experiments with JUCE. |
| `Source/`         | Top-level directory for Spark's source code. |
| `mixdowns-example/` | An example folder holding mixdowns. For use in quickly testing Spark. |
| `Spark.jucer`     | JUCE project configuration. Necessary for managing Spark through the Projucer. |

## Building and Developing with JUCE
Spark is largely a [JUCE](https://juce.com/)-based project. Since JUCE is such a heavy framework, building and running Spark (as well as our prototypes) requires first setting up and getting a little familiar with JUCE and its project manager, Projucer. Follow [this JUCE guide](https://docs.juce.com/master/tutorial_new_projucer_project.html) to do so. Once JUCE is installed, you can open a `*.jucer` file using the Projucer to setup a project and open it in a supported IDE of your choice, from where you can easily build the project directly within the IDE. 

If you don't want to build the project using an IDE (which is the recommended way for developers), you can also build this project using CMake with Ninja on all of Windows, Mac and Linux. We heavily recommend using [FRUT](https://github.com/McMartin/FRUT) to generate the CMakeLists.txt file for this project and following the instructions listed there.

## Testing
Currently, our testing is done through a separate JUCE project, as the JUCE unit testing framework requires us to do so. The testing project's repository is located at: https://github.com/jaygrinols/Spark-Testing
To create a new test in this repository:
Using projucer, add a new header file. This header file you created will include your actual unit test.
As per the [JUCE UnitTest Class Documentation](https://docs.juce.com/master/classUnitTest.html), initialize this header file with a class following this structure:
```
class MyTest  : public UnitTest
{
public:
    MyTest()  : UnitTest ("Foobar testing") {}
 
    void runTest() override
    {
        beginTest ("Part 1");
 
        expect (myFoobar.doesSomething());
        expect (myFoobar.doesSomethingElse());
 
        beginTest ("Part 2");
 
        expect (myOtherFoobar.doesSomething());
        expect (myOtherFoobar.doesSomethingElse());
 
        ...etc..
    }
};
```
However, unlike the documentation, instead of initializing a static instance of the class at the bottom of the file, follow these steps: 
1. Navigate to main.cpp of the Test Suite project
2. Include your created header file in main.cpp
3. Navigate to the comment that says "Add your tests here." in main.cpp
4. Instantiate and run your test class here.

Then, simply build and run your project using your IDE of choice to run your tests.

### macOS
In order for JUCE to access audio input devices, it needs to register and receive permission. To run and debug with audio input, enable Microphone Access for your specific project and exporter within the Projucer.

### Other quirks of JUCE development
As stated in [this JUCE tutorial](https://docs.juce.com/master/tutorial_main_component.html), you should "always use The Projucer to create new files; never do so from your IDE (The Projucer would overwrite such changes the next time you save your project)."
