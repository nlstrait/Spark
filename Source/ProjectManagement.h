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
    Layer(juce::File& layerFile);
    ~Layer();
    
    bool operator==(const Layer& other);

private:
    juce::File& layerFile;
};


class Project {
public:
    /**
     Create a new Project.
     @param mixdownFile The file in which this Project's mixdown is stored.
     */
    Project(juce::File& mixdownFile);
    
    /**
     Deconstruct this Project and permanently delete discarded files.
     */
    ~Project();
    
    /**
     Adds a layer to this Project.
     @param layerFile    The file in which the layer to add is stored.
     */
    void addLayer(Layer layer);
    
    /**
     Removes a layer from this Project, without discarding the audio completely.
     @param layerFile    The file in which the layer to remove is stored.
     */
    void removeLayer(juce::File& layerFile);
    
private:
    Layer* layers;
};


class ProjectManager {
public:
    /**
     Create a new ProjectManager and initialize Projects
     @param mixdownFolder   The folder which this ProjectManager will manage and identify Projects in.
     */
    ProjectManager(juce::File& mixdownFolder);
    ~ProjectManager();
    
    void getCurrentProject();
    void getNextProject();
    void getPreviousProject();

private:
    Project* projects;
    int currentProjectIndex = -1;
};

