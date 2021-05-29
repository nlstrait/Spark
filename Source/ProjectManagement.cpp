/*
  ==============================================================================

    ProjectManagement.cpp
    Created: 17 May 2021 6:29:37pm
    Author:  Nolan Strait

  ==============================================================================
*/

#include <JuceHeader.h>

#include "ProjectManagement.h"


//===================================== Layer =========================================

Layer::Layer(juce::File file) : layerFile(file) {}

bool Layer::operator==(const Layer &other) {
    return layerFile == other.layerFile;
}

juce::File& Layer::getFile() { return layerFile; }


//===================================== Project =========================================

// This seems like bad practice but was necessary to get working with juce::Array. Normally,
// I would not create a default constructor like this. This should never be called. - Nolan
Project::Project() : layers(), mixdownFile(juce::File()) {}

Project::Project(juce::File& mixdownFile) : mixdownFile(mixdownFile) {
    if (mixdownFile.isDirectory()) throw "Invalid mixdownFile given; mixdownFile is a directory";
    // Check this mixdownFile's containing directory for a sub-directory with the same name
    // as the mixdownFile (minus the audio file extension).
    juce::File layersDir = mixdownFile.withFileExtension(juce::StringRef()); // potential layers directory
    if (layersDir.isDirectory()) {
        juce::Array<juce::File> children = layersDir.findChildFiles(juce::File::findFiles, false);
        for (juce::File child : children) {
            layers.add(Layer(child));
        }
    }
}

juce::String Project::getName() {
    return mixdownFile.getFileNameWithoutExtension();
}

juce::File& Project::getMixdownFile() { return mixdownFile; }

juce::File Project::getLayerDirectory() {
    return mixdownFile.withFileExtension(juce::StringRef());
}

juce::File Project::createNewLayer() {
    juce::File layerFile = getLayerDirectory().getNonexistentChildFile("layer_" + std::to_string(getNumLayers() + 1), ".wav");
    layerFile.create();
    layers.add(Layer(layerFile));
    return layerFile;
}

juce::File Project::getLastLayerFile() {
    return layers.getReference(layers.size() - 1).getFile();
}

int Project::getNumLayers() { return layers.size(); }


//===================================== ProjectManagement =========================================

juce::Array<Project> ProjectManagement::getAllProjectsInFolder(juce::File &mixdownFolder) {
    juce::Array<Project> projects;
    juce::Array<juce::File> children = mixdownFolder.findChildFiles(juce::File::findFiles, false);
    for (juce::File child : children) {
        if (child.getFileExtension().equalsIgnoreCase(".wav"))
            projects.add(Project(child));
    }
    return projects;
}
