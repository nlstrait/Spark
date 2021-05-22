/*
  ==============================================================================

    ProjectManagement.cpp
    Created: 17 May 2021 6:29:37pm
    Author:  Nolan Strait

  ==============================================================================
*/

#include <JuceHeader.h>

#include "ProjectManagement.h"

Layer::Layer(juce::File& file) : layerFile(file) {}

bool Layer::operator==(const Layer &other) {
    return layerFile == other.layerFile;
}

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

juce::Array<Project> ProjectManagement::getAllProjectsInFolder(juce::File &mixdownFolder) {
    juce::Array<Project> projects;
    juce::Array<juce::File> children = mixdownFolder.findChildFiles(juce::File::findFiles, false);
    for (juce::File child : children) {
        projects.add(Project(child));
    }
    return projects;
}
