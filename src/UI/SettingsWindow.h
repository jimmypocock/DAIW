#pragma once

#include <JuceHeader.h>
#include "AudioSettingsPanel.h"
#include "LookAndFeel/DAIWLookAndFeel.h"

class AudioEngine;

/**
 * SettingsWindow is a modal overlay with sidebar navigation.
 *
 * Contains settings panels for Audio, AI, and General settings.
 */
class SettingsWindow : public juce::Component
{
public:
    SettingsWindow(AudioEngine& audioEngine);
    ~SettingsWindow() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Handle mouse clicks (close when clicking outside panel)
    void mouseDown(const juce::MouseEvent& event) override;

    // Handle escape key
    bool keyPressed(const juce::KeyPress& key) override;

    // Show/hide the settings window
    void show();
    void hide();
    bool isShowing() const { return isVisible(); }

private:
    // Sidebar button for navigation
    class SidebarButton : public juce::TextButton
    {
    public:
        SidebarButton(const juce::String& name);
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;
    };

    void selectPanel(int index);
    juce::Rectangle<int> getPanelBounds() const;

    AudioEngine& audioEngine;

    // UI Components
    juce::Label titleLabel;
    juce::TextButton closeButton;

    // Sidebar
    std::vector<std::unique_ptr<SidebarButton>> sidebarButtons;
    int selectedPanelIndex = 0;

    // Content panels
    std::unique_ptr<AudioSettingsPanel> audioSettingsPanel;

    // Layout constants
    static constexpr int panelWidth = 600;
    static constexpr int panelHeight = 450;
    static constexpr int sidebarWidth = 150;
    static constexpr int headerHeight = 50;
    static constexpr int padding = 20;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsWindow)
};
