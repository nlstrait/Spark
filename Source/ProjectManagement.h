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
    Layer(juce::File file);
    bool operator==(const Layer& other);
    
    juce::File& getFile();
    
private:
    juce::File layerFile;
};


class Project {
public:
    
    Project();
    
    /**
     Create a new Project.
     @throws Invalid mixdownFile exception if mixdownFile is a directory.
     @param mixdownFile The file in which this Project's mixdown is stored.
     */
    Project(juce::File& mixdownFile);
    
    /**
     Return the name of this Project, which is based on its mixdown.
     */
    juce::String getName();
    
    /**
     Retrieve this Project's mixdown file.
     @return A mixdown file.
     */
    juce::File& getMixdownFile();
    
    /**
     Retrieve the directory where this Project's Layers live.
     @return A directory.
     */
    juce::File getLayerDirectory();
    
    /**
     Creates and returns a new layer for this Project.
     */
    juce::File createNewLayer();
    
    /**
     Return the number of Layers within this Project.
     */
    int getNumLayers();
    
    // The layers which this Project contains. Open for manipulation from outsiders,
    // at least for now, to make for ease of use.
    juce::Array<Layer> layers;
    
private:
    juce::File mixdownFile;
};


/**
 Utilities for generating and managing collections of projects.
 */
class ProjectManagement {
public:
    /**
     Generate Projects from a given mixdown folder. Assumes that the folder specified has mixdown audio files in its root
     and layers for each project in subfolders with names identicle to their associated mixdown file (minus the file extension).
     
     For example, with a mixdown named "mixd.wav", we expect a subfolder within "mixd.wav"'s containing folder to be named
     "mixd/" and contain the audio layers for the mixd project. If there is no such subfolder, it is assumed that this project
     has no layers (yet).
     
     @param mixdownFolder   The folder to generate mixdowns from.
     @return    A list of Projects found in given directory.
     */
    static juce::Array<Project> getAllProjectsInFolder(juce::File& mixdownFolder);
};
