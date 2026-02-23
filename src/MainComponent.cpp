#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1280, 800);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(48.0f).withStyle("Bold"));
    g.drawText("DAIW", getLocalBounds().removeFromTop(getHeight() / 2 + 30),
               juce::Justification::centredBottom);

    // Subtitle
    g.setFont(juce::FontOptions(18.0f));
    g.setColour(juce::Colour(0xff888888));
    g.drawText("Digital AI Workstation", getLocalBounds().removeFromBottom(getHeight() / 2),
               juce::Justification::centredTop);
}

void MainComponent::resized()
{
    // Layout child components here
}
