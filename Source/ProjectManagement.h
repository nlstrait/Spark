/*
  ==============================================================================

    ProjectManager.h
    Created: 16 May 2021 6:28:18pm
    Author:  Nolan Strait

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class Layer {
public:
    Layer(juce::File& file);
    bool operator==(const Layer& other);
private:
    juce::File& layerFile;
};

class Project {
public:
    /**
     Create a new Project.
     @throws Invalid mixdownFile exception if mixdownFile is a directory.
     @param mixdownFile The file in which this Project's mixdown is stored.
     */
    Project(juce::File& mixdownFile);
    
    // The layers which this Project contains. Open for manipulation from outsiders,
    // at least for now, to make for ease of use.
    juce::Array<Layer> layers;
    
private:
    juce::File& mixdownFile;
};

/**
 Utilities for generating and managing collections of projects.
 */
class ProjectManagement {
public:
    /**
     Generate Projects from a given mixdown folder. Assumes that the folder specified has mixdown audio files in its root
     and layers for each project in subfolders with names identicle to their associated mixdown file (minus the file extension).
     @param mixdownFolder   The folder to generate mixdowns from.
     @return    A list of Projects found in given directory.
     */
    static juce::Array<Project> getAllProjectsInFolder(juce::File& mixdownFolder);
};
