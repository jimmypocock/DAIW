#pragma once

#include <JuceHeader.h>

class AudioEngine;

/**
 * AudioSettingsPanel displays audio device configuration.
 *
 * - Input device selection
 * - Output device selection
 * - Sample rate selection
 * - Buffer size selection
 */
class AudioSettingsPanel : public juce::Component, private juce::ChangeListener
{
public:
    explicit AudioSettingsPanel(AudioEngine& audioEngine);
    ~AudioSettingsPanel() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void refreshDeviceLists();
    void refreshCurrentSettings();

    void inputDeviceChanged();
    void outputDeviceChanged();
    void sampleRateChanged();
    void bufferSizeChanged();

    AudioEngine& audioEngine;

    // Section label
    juce::Label sectionLabel;

    // Input device
    juce::Label inputLabel;
    juce::ComboBox inputDeviceCombo;

    // Output device
    juce::Label outputLabel;
    juce::ComboBox outputDeviceCombo;

    // Sample rate
    juce::Label sampleRateLabel;
    juce::ComboBox sampleRateCombo;

    // Buffer size
    juce::Label bufferSizeLabel;
    juce::ComboBox bufferSizeCombo;

    // Status
    juce::Label statusLabel;

    // Layout constants
    static constexpr int rowHeight = 36;
    static constexpr int labelWidth = 120;
    static constexpr int comboWidth = 250;
    static constexpr int spacing = 12;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsPanel)
};
