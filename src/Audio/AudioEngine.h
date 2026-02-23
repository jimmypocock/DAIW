#pragma once

#include <JuceHeader.h>
#include <atomic>

/**
 * AudioEngine manages audio device I/O and the core audio processing.
 *
 * Currently implements simple passthrough (input -> output).
 * Will eventually handle tracks, mixing, and plugin processing.
 */
class AudioEngine : public juce::AudioSource
{
public:
    AudioEngine();
    ~AudioEngine() override;

    // AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Device management
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

    // Device info
    juce::StringArray getAvailableInputDevices();
    juce::StringArray getAvailableOutputDevices();
    juce::String getCurrentInputDevice() const;
    juce::String getCurrentOutputDevice() const;

    // Device selection
    juce::String setInputDevice(const juce::String& deviceName);
    juce::String setOutputDevice(const juce::String& deviceName);

    // Audio settings
    double getSampleRate() const;
    int getBufferSize() const;
    juce::String setSampleRate(double sampleRate);
    juce::String setBufferSize(int bufferSize);

    juce::Array<double> getAvailableSampleRates() const;
    juce::Array<int> getAvailableBufferSizes() const;

    // Start/stop audio
    void start();
    void stop();
    bool isRunning() const { return running; }

    // Audio levels (thread-safe, for UI display)
    float getInputLevelLeft() const { return inputLevelLeft.load(); }
    float getInputLevelRight() const { return inputLevelRight.load(); }
    float getOutputLevelLeft() const { return outputLevelLeft.load(); }
    float getOutputLevelRight() const { return outputLevelRight.load(); }

private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer sourcePlayer;

    double currentSampleRate = 0.0;
    int currentBufferSize = 0;
    bool running = false;

    // Audio levels (atomic for thread safety between audio and UI threads)
    std::atomic<float> inputLevelLeft{0.0f};
    std::atomic<float> inputLevelRight{0.0f};
    std::atomic<float> outputLevelLeft{0.0f};
    std::atomic<float> outputLevelRight{0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
