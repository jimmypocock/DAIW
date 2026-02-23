#pragma once

#include <JuceHeader.h>
#include "Audio/AudioEngine.h"
#include "UI/LookAndFeel/DAIWLookAndFeel.h"
#include "UI/Components/LevelMeter.h"
#include "UI/SettingsWindow.h"

class MainComponent : public juce::Component, public juce::ApplicationCommandTarget, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Command IDs
    enum CommandIDs
    {
        openSettings = 0x1001
    };

    // ApplicationCommandTarget interface
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    juce::ApplicationCommandManager& getCommandManager() { return commandManager; }

private:
    void timerCallback() override;

    DAIWLookAndFeel lookAndFeel;
    AudioEngine audioEngine;
    SettingsWindow settingsWindow;

    // Level meters
    StereoLevelMeter inputMeter;
    StereoLevelMeter outputMeter;

    juce::ApplicationCommandManager commandManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
