#include "AudioSettingsPanel.h"
#include "../Audio/AudioEngine.h"
#include "LookAndFeel/DAIWLookAndFeel.h"

AudioSettingsPanel::AudioSettingsPanel(AudioEngine& engine) : audioEngine(engine)
{
    // Listen for device changes
    audioEngine.getDeviceManager().addChangeListener(this);

    // Section header
    sectionLabel.setText("Audio Device Configuration", juce::dontSendNotification);
    sectionLabel.setFont(juce::Font(juce::FontOptions(16.0f).withStyle("Bold")));
    sectionLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textPrimary);
    addAndMakeVisible(sectionLabel);

    // Input device
    inputLabel.setText("Input Device", juce::dontSendNotification);
    inputLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textSecondary);
    addAndMakeVisible(inputLabel);

    inputDeviceCombo.onChange = [this]() { inputDeviceChanged(); };
    addAndMakeVisible(inputDeviceCombo);

    // Output device
    outputLabel.setText("Output Device", juce::dontSendNotification);
    outputLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textSecondary);
    addAndMakeVisible(outputLabel);

    outputDeviceCombo.onChange = [this]() { outputDeviceChanged(); };
    addAndMakeVisible(outputDeviceCombo);

    // Sample rate
    sampleRateLabel.setText("Sample Rate", juce::dontSendNotification);
    sampleRateLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textSecondary);
    addAndMakeVisible(sampleRateLabel);

    sampleRateCombo.onChange = [this]() { sampleRateChanged(); };
    addAndMakeVisible(sampleRateCombo);

    // Buffer size
    bufferSizeLabel.setText("Buffer Size", juce::dontSendNotification);
    bufferSizeLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textSecondary);
    addAndMakeVisible(bufferSizeLabel);

    bufferSizeCombo.onChange = [this]() { bufferSizeChanged(); };
    addAndMakeVisible(bufferSizeCombo);

    // Status label
    statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textMuted);
    statusLabel.setFont(juce::Font(juce::FontOptions(12.0f)));
    addAndMakeVisible(statusLabel);

    // Initial population
    refreshDeviceLists();
    refreshCurrentSettings();
}

AudioSettingsPanel::~AudioSettingsPanel()
{
    audioEngine.getDeviceManager().removeChangeListener(this);
}

void AudioSettingsPanel::paint(juce::Graphics& /*g*/)
{
    // Background handled by parent
}

void AudioSettingsPanel::resized()
{
    auto bounds = getLocalBounds();

    // Section header
    sectionLabel.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(spacing);

    // Input device row
    auto inputRow = bounds.removeFromTop(rowHeight);
    inputLabel.setBounds(inputRow.removeFromLeft(labelWidth));
    inputDeviceCombo.setBounds(inputRow.removeFromLeft(comboWidth));
    bounds.removeFromTop(spacing);

    // Output device row
    auto outputRow = bounds.removeFromTop(rowHeight);
    outputLabel.setBounds(outputRow.removeFromLeft(labelWidth));
    outputDeviceCombo.setBounds(outputRow.removeFromLeft(comboWidth));
    bounds.removeFromTop(spacing);

    // Sample rate row
    auto sampleRateRow = bounds.removeFromTop(rowHeight);
    sampleRateLabel.setBounds(sampleRateRow.removeFromLeft(labelWidth));
    sampleRateCombo.setBounds(sampleRateRow.removeFromLeft(comboWidth));
    bounds.removeFromTop(spacing);

    // Buffer size row
    auto bufferSizeRow = bounds.removeFromTop(rowHeight);
    bufferSizeLabel.setBounds(bufferSizeRow.removeFromLeft(labelWidth));
    bufferSizeCombo.setBounds(bufferSizeRow.removeFromLeft(comboWidth));
    bounds.removeFromTop(spacing * 2);

    // Status
    statusLabel.setBounds(bounds.removeFromTop(20));
}

void AudioSettingsPanel::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{
    // Device configuration changed externally
    refreshCurrentSettings();
}

void AudioSettingsPanel::refreshDeviceLists()
{
    // Input devices
    inputDeviceCombo.clear();
    auto inputDevices = audioEngine.getAvailableInputDevices();
    int itemId = 1;
    for (const auto& device : inputDevices)
    {
        inputDeviceCombo.addItem(device, itemId++);
    }

    // Output devices
    outputDeviceCombo.clear();
    auto outputDevices = audioEngine.getAvailableOutputDevices();
    itemId = 1;
    for (const auto& device : outputDevices)
    {
        outputDeviceCombo.addItem(device, itemId++);
    }
}

void AudioSettingsPanel::refreshCurrentSettings()
{
    // Select current input device
    auto currentInput = audioEngine.getCurrentInputDevice();
    for (int i = 0; i < inputDeviceCombo.getNumItems(); ++i)
    {
        if (inputDeviceCombo.getItemText(i) == currentInput)
        {
            inputDeviceCombo.setSelectedItemIndex(i, juce::dontSendNotification);
            break;
        }
    }

    // Select current output device
    auto currentOutput = audioEngine.getCurrentOutputDevice();
    for (int i = 0; i < outputDeviceCombo.getNumItems(); ++i)
    {
        if (outputDeviceCombo.getItemText(i) == currentOutput)
        {
            outputDeviceCombo.setSelectedItemIndex(i, juce::dontSendNotification);
            break;
        }
    }

    // Sample rates
    sampleRateCombo.clear();
    auto sampleRates = audioEngine.getAvailableSampleRates();
    auto currentSampleRate = audioEngine.getSampleRate();
    int selectedIndex = 0;
    int itemId = 1;

    for (int i = 0; i < sampleRates.size(); ++i)
    {
        auto rate = sampleRates[i];
        sampleRateCombo.addItem(juce::String(static_cast<int>(rate)) + " Hz", itemId++);
        if (std::abs(rate - currentSampleRate) < 1.0)
        {
            selectedIndex = i;
        }
    }
    sampleRateCombo.setSelectedItemIndex(selectedIndex, juce::dontSendNotification);

    // Buffer sizes
    bufferSizeCombo.clear();
    auto bufferSizes = audioEngine.getAvailableBufferSizes();
    auto currentBufferSize = audioEngine.getBufferSize();
    selectedIndex = 0;
    itemId = 1;

    for (int i = 0; i < bufferSizes.size(); ++i)
    {
        auto size = bufferSizes[i];
        // Calculate latency in ms
        auto latencyMs = (currentSampleRate > 0)
                             ? (static_cast<double>(size) / currentSampleRate * 1000.0)
                             : 0.0;

        juce::String label = juce::String(size) + " samples";
        if (latencyMs > 0)
        {
            label += " (" + juce::String(latencyMs, 1) + " ms)";
        }

        bufferSizeCombo.addItem(label, itemId++);

        if (size == currentBufferSize)
        {
            selectedIndex = i;
        }
    }
    bufferSizeCombo.setSelectedItemIndex(selectedIndex, juce::dontSendNotification);

    // Update status
    if (currentSampleRate > 0 && currentBufferSize > 0)
    {
        auto latencyMs = static_cast<double>(currentBufferSize) / currentSampleRate * 1000.0;
        statusLabel.setText("Latency: " + juce::String(latencyMs, 1) + " ms",
                            juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textMuted);
    }
    else
    {
        statusLabel.setText("No audio device selected", juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::warning);
    }
}

void AudioSettingsPanel::inputDeviceChanged()
{
    auto deviceName = inputDeviceCombo.getText();
    DBG("Attempting to set input device to: " + deviceName);

    statusLabel.setText("Setting input device...", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::primaryAccent);

    auto error = audioEngine.setInputDevice(deviceName);

    if (error.isNotEmpty())
    {
        DBG("Failed to set input device: " + error);
        statusLabel.setText("Error: " + error, juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::error);
        // Refresh to show the actual current device
        refreshCurrentSettings();
    }
    else
    {
        DBG("Successfully set input device to: " + deviceName);
        // Don't call refreshCurrentSettings() here - let the change listener handle it
        // Just update the status
        auto latencyMs = static_cast<double>(audioEngine.getBufferSize()) /
                         audioEngine.getSampleRate() * 1000.0;
        statusLabel.setText("Input: " + deviceName + " | Latency: " +
                                juce::String(latencyMs, 1) + " ms",
                            juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::success);
    }
}

void AudioSettingsPanel::outputDeviceChanged()
{
    auto deviceName = outputDeviceCombo.getText();
    DBG("Attempting to set output device to: " + deviceName);

    statusLabel.setText("Setting output device...", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::primaryAccent);

    auto error = audioEngine.setOutputDevice(deviceName);

    if (error.isNotEmpty())
    {
        DBG("Failed to set output device: " + error);
        statusLabel.setText("Error: " + error, juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::error);
        // Refresh to show the actual current device
        refreshCurrentSettings();
    }
    else
    {
        DBG("Successfully set output device to: " + deviceName);
        auto latencyMs = static_cast<double>(audioEngine.getBufferSize()) /
                         audioEngine.getSampleRate() * 1000.0;
        statusLabel.setText("Output: " + deviceName + " | Latency: " +
                                juce::String(latencyMs, 1) + " ms",
                            juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::success);
    }
}

void AudioSettingsPanel::sampleRateChanged()
{
    auto sampleRates = audioEngine.getAvailableSampleRates();
    auto index = sampleRateCombo.getSelectedItemIndex();

    if (index >= 0 && index < sampleRates.size())
    {
        auto error = audioEngine.setSampleRate(sampleRates[index]);

        if (error.isNotEmpty())
        {
            DBG("Failed to set sample rate: " + error);
            statusLabel.setText("Error: " + error, juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::error);
        }
        else
        {
            refreshCurrentSettings();
        }
    }
}

void AudioSettingsPanel::bufferSizeChanged()
{
    auto bufferSizes = audioEngine.getAvailableBufferSizes();
    auto index = bufferSizeCombo.getSelectedItemIndex();

    if (index >= 0 && index < bufferSizes.size())
    {
        auto error = audioEngine.setBufferSize(bufferSizes[index]);

        if (error.isNotEmpty())
        {
            DBG("Failed to set buffer size: " + error);
            statusLabel.setText("Error: " + error, juce::dontSendNotification);
            statusLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::error);
        }
        else
        {
            refreshCurrentSettings();
        }
    }
}
