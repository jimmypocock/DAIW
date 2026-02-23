#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    // Initialize with default audio device
    auto result = deviceManager.initialiseWithDefaultDevices(2, 2);

    if (result.isNotEmpty())
    {
        DBG("AudioEngine: Failed to initialize audio devices: " + result);
    }
    else
    {
        // Try to set a lower buffer size for reduced latency
        auto setup = deviceManager.getAudioDeviceSetup();
        setup.bufferSize = 256; // Lower latency (~5ms at 48kHz)
        auto bufferResult = deviceManager.setAudioDeviceSetup(setup, true);

        if (bufferResult.isNotEmpty())
        {
            DBG("AudioEngine: Could not set preferred buffer size: " + bufferResult);
        }
        else
        {
            DBG("AudioEngine: Set buffer size to 256 samples for lower latency");
        }
    }

    // Connect the source player to this AudioSource
    sourcePlayer.setSource(this);
}

AudioEngine::~AudioEngine()
{
    stop();
    sourcePlayer.setSource(nullptr);
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    currentBufferSize = samplesPerBlockExpected;

    DBG("AudioEngine: Prepared to play - Sample rate: " + juce::String(sampleRate) +
        ", Buffer size: " + juce::String(samplesPerBlockExpected));
}

void AudioEngine::releaseResources()
{
    DBG("AudioEngine: Released resources");
}

void AudioEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();

    if (device == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        inputLevelLeft.store(0.0f);
        inputLevelRight.store(0.0f);
        outputLevelLeft.store(0.0f);
        outputLevelRight.store(0.0f);
        return;
    }

    auto activeInputChannels = device->getActiveInputChannels();
    auto numChannels = bufferToFill.buffer->getNumChannels();

    // Calculate input levels (RMS)
    if (numChannels > 0 && activeInputChannels[0])
    {
        auto level = bufferToFill.buffer->getRMSLevel(0, bufferToFill.startSample,
                                                       bufferToFill.numSamples);
        inputLevelLeft.store(level);
    }
    else
    {
        inputLevelLeft.store(0.0f);
    }

    if (numChannels > 1 && activeInputChannels[1])
    {
        auto level = bufferToFill.buffer->getRMSLevel(1, bufferToFill.startSample,
                                                       bufferToFill.numSamples);
        inputLevelRight.store(level);
    }
    else
    {
        inputLevelRight.store(0.0f);
    }

    // If no input, clear output to silence
    if (activeInputChannels.isZero())
    {
        bufferToFill.clearActiveBufferRegion();
        inputLevelLeft.store(0.0f);
        inputLevelRight.store(0.0f);
        outputLevelLeft.store(0.0f);
        outputLevelRight.store(0.0f);
        return;
    }

    // Passthrough: input is already in the buffer
    // For channels that have no input, clear them
    for (int channel = 0; channel < numChannels; ++channel)
    {
        if (!activeInputChannels[channel])
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample,
                                       bufferToFill.numSamples);
        }
    }

    // Output levels (same as input for passthrough)
    outputLevelLeft.store(inputLevelLeft.load());
    outputLevelRight.store(inputLevelRight.load());
}

void AudioEngine::start()
{
    if (!running)
    {
        deviceManager.addAudioCallback(&sourcePlayer);
        running = true;
        DBG("AudioEngine: Started");
    }
}

void AudioEngine::stop()
{
    if (running)
    {
        deviceManager.removeAudioCallback(&sourcePlayer);
        running = false;
        DBG("AudioEngine: Stopped");
    }
}

juce::StringArray AudioEngine::getAvailableInputDevices()
{
    juce::StringArray devices;
    auto& types = deviceManager.getAvailableDeviceTypes();

    for (auto* type : types)
    {
        auto deviceNames = type->getDeviceNames(true); // true = input devices
        devices.addArray(deviceNames);
    }

    return devices;
}

juce::StringArray AudioEngine::getAvailableOutputDevices()
{
    juce::StringArray devices;
    auto& types = deviceManager.getAvailableDeviceTypes();

    for (auto* type : types)
    {
        auto deviceNames = type->getDeviceNames(false); // false = output devices
        devices.addArray(deviceNames);
    }

    return devices;
}

juce::String AudioEngine::getCurrentInputDevice() const
{
    auto* setup = deviceManager.getCurrentAudioDevice();
    if (setup != nullptr)
    {
        auto audioSetup = deviceManager.getAudioDeviceSetup();
        return audioSetup.inputDeviceName;
    }
    return {};
}

juce::String AudioEngine::getCurrentOutputDevice() const
{
    auto* setup = deviceManager.getCurrentAudioDevice();
    if (setup != nullptr)
    {
        auto audioSetup = deviceManager.getAudioDeviceSetup();
        return audioSetup.outputDeviceName;
    }
    return {};
}

juce::String AudioEngine::setInputDevice(const juce::String& deviceName)
{
    DBG("AudioEngine::setInputDevice called with: " + deviceName);

    auto setup = deviceManager.getAudioDeviceSetup();
    DBG("Current setup - Input: " + setup.inputDeviceName + ", Output: " + setup.outputDeviceName);

    setup.inputDeviceName = deviceName;
    setup.useDefaultInputChannels = true;

    auto result = deviceManager.setAudioDeviceSetup(setup, true);

    if (result.isEmpty())
    {
        DBG("AudioEngine::setInputDevice succeeded");
        auto newSetup = deviceManager.getAudioDeviceSetup();
        DBG("New setup - Input: " + newSetup.inputDeviceName + ", Output: " + newSetup.outputDeviceName);
    }
    else
    {
        DBG("AudioEngine::setInputDevice failed: " + result);
    }

    return result;
}

juce::String AudioEngine::setOutputDevice(const juce::String& deviceName)
{
    DBG("AudioEngine::setOutputDevice called with: " + deviceName);

    auto setup = deviceManager.getAudioDeviceSetup();
    DBG("Current setup - Input: " + setup.inputDeviceName + ", Output: " + setup.outputDeviceName);

    setup.outputDeviceName = deviceName;
    setup.useDefaultOutputChannels = true;

    auto result = deviceManager.setAudioDeviceSetup(setup, true);

    if (result.isEmpty())
    {
        DBG("AudioEngine::setOutputDevice succeeded");
        auto newSetup = deviceManager.getAudioDeviceSetup();
        DBG("New setup - Input: " + newSetup.inputDeviceName + ", Output: " + newSetup.outputDeviceName);
    }
    else
    {
        DBG("AudioEngine::setOutputDevice failed: " + result);
    }

    return result;
}

double AudioEngine::getSampleRate() const
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device != nullptr)
    {
        return device->getCurrentSampleRate();
    }
    return 0.0;
}

int AudioEngine::getBufferSize() const
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device != nullptr)
    {
        return device->getCurrentBufferSizeSamples();
    }
    return 0;
}

juce::String AudioEngine::setSampleRate(double sampleRate)
{
    auto setup = deviceManager.getAudioDeviceSetup();
    setup.sampleRate = sampleRate;
    return deviceManager.setAudioDeviceSetup(setup, true);
}

juce::String AudioEngine::setBufferSize(int bufferSize)
{
    auto setup = deviceManager.getAudioDeviceSetup();
    setup.bufferSize = bufferSize;
    return deviceManager.setAudioDeviceSetup(setup, true);
}

juce::Array<double> AudioEngine::getAvailableSampleRates() const
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device != nullptr)
    {
        return device->getAvailableSampleRates();
    }
    return {};
}

juce::Array<int> AudioEngine::getAvailableBufferSizes() const
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device != nullptr)
    {
        return device->getAvailableBufferSizes();
    }
    return {};
}
