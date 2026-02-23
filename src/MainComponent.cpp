#include "MainComponent.h"

MainComponent::MainComponent() : settingsWindow(audioEngine)
{
    // Apply custom look and feel
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

    // Setup command manager
    commandManager.registerAllCommandsForTarget(this);

    // Add key mappings
    auto* keyMappings = commandManager.getKeyMappings();
    keyMappings->addKeyPress(openSettings,
                              juce::KeyPress(',', juce::ModifierKeys::commandModifier, 0));

    // Setup level meters
    inputMeter.setLabel("INPUT");
    addAndMakeVisible(inputMeter);

    outputMeter.setLabel("OUTPUT");
    addAndMakeVisible(outputMeter);

    // Add settings window as child (invisible by default)
    addChildComponent(settingsWindow);

    // Start audio engine
    audioEngine.start();

    // Start timer for level meter updates
    startTimerHz(30);

    setSize(1280, 800);

    // Set this component as the command target
    commandManager.setFirstCommandTarget(this);
}

MainComponent::~MainComponent()
{
    stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    audioEngine.stop();
}

void MainComponent::timerCallback()
{
    // Update level meters with current audio levels
    inputMeter.setLevels(audioEngine.getInputLevelLeft(), audioEngine.getInputLevelRight());
    outputMeter.setLevels(audioEngine.getOutputLevelLeft(), audioEngine.getOutputLevelRight());
}

void MainComponent::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(DAIWLookAndFeel::Colors::background);

    // Only draw content if settings not visible
    if (!settingsWindow.isShowing())
    {
        // Title
        g.setColour(DAIWLookAndFeel::Colors::textPrimary);
        g.setFont(juce::Font(juce::FontOptions(48.0f).withStyle("Bold")));
        g.drawText("DAIW", getLocalBounds().removeFromTop(getHeight() / 2),
                   juce::Justification::centredBottom);

        // Subtitle
        g.setFont(juce::Font(juce::FontOptions(18.0f)));
        g.setColour(DAIWLookAndFeel::Colors::textSecondary);
        g.drawText("Digital AI Workstation",
                   getLocalBounds().withTrimmedTop(getHeight() / 2).removeFromTop(40),
                   juce::Justification::centredTop);

        // Instructions
        g.setFont(juce::Font(juce::FontOptions(14.0f)));
        g.setColour(DAIWLookAndFeel::Colors::textMuted);
        g.drawText("Press " + juce::String(juce::CharPointer_UTF8("\xe2\x8c\x98")) +
                       ", to open Settings",
                   getLocalBounds().removeFromBottom(60), juce::Justification::centredTop);
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    // Position level meters on the right side
    auto meterArea = bounds.removeFromRight(100).reduced(20, 100);

    // Split meter area for input and output
    auto inputArea = meterArea.removeFromLeft(35);
    meterArea.removeFromLeft(10); // Gap
    auto outputArea = meterArea.removeFromLeft(35);

    inputMeter.setBounds(inputArea);
    outputMeter.setBounds(outputArea);

    // Settings window fills entire component
    settingsWindow.setBounds(getLocalBounds());
}

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
    return nullptr;
}

void MainComponent::getAllCommands(juce::Array<juce::CommandID>& commands)
{
    commands.add(openSettings);
}

void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case openSettings:
            result.setInfo("Settings...", "Open the settings window", "Application", 0);
            result.addDefaultKeypress(',', juce::ModifierKeys::commandModifier);
            break;
        default:
            break;
    }
}

bool MainComponent::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
        case openSettings:
            if (settingsWindow.isShowing())
            {
                settingsWindow.hide();
            }
            else
            {
                settingsWindow.show();
            }
            repaint();
            return true;
        default:
            return false;
    }
}
