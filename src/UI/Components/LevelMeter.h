#pragma once

#include <JuceHeader.h>
#include "../LookAndFeel/DAIWLookAndFeel.h"

/**
 * LevelMeter displays audio levels with smooth decay.
 *
 * Can display as vertical or horizontal bar.
 */
class LevelMeter : public juce::Component, private juce::Timer
{
public:
    LevelMeter();
    ~LevelMeter() override = default;

    void paint(juce::Graphics& g) override;

    // Set the current level (0.0 to 1.0)
    void setLevel(float newLevel);

    // Set whether the meter is vertical (default) or horizontal
    void setVertical(bool shouldBeVertical) { vertical = shouldBeVertical; }

private:
    void timerCallback() override;

    float currentLevel = 0.0f;
    float displayLevel = 0.0f;
    bool vertical = true;

    // Decay rate (how fast the meter falls)
    static constexpr float decayRate = 0.92f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};

/**
 * StereoLevelMeter displays left and right channel levels.
 */
class StereoLevelMeter : public juce::Component, private juce::Timer
{
public:
    StereoLevelMeter();
    ~StereoLevelMeter() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Set levels for left and right channels (0.0 to 1.0)
    void setLevels(float left, float right);

    // Set label
    void setLabel(const juce::String& text) { label = text; }

private:
    void timerCallback() override;

    LevelMeter leftMeter;
    LevelMeter rightMeter;
    juce::String label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoLevelMeter)
};
