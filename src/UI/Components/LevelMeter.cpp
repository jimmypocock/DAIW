#include "LevelMeter.h"

//==============================================================================
// LevelMeter
//==============================================================================

LevelMeter::LevelMeter()
{
    startTimerHz(30); // 30 fps update
}

void LevelMeter::setLevel(float newLevel)
{
    currentLevel = juce::jlimit(0.0f, 1.0f, newLevel);

    // Peak hold - only update display if new level is higher
    if (currentLevel > displayLevel)
    {
        displayLevel = currentLevel;
    }
}

void LevelMeter::timerCallback()
{
    // Decay the display level
    if (displayLevel > currentLevel)
    {
        displayLevel *= decayRate;
        if (displayLevel < 0.001f)
        {
            displayLevel = 0.0f;
        }
        repaint();
    }
    else if (displayLevel < currentLevel)
    {
        displayLevel = currentLevel;
        repaint();
    }
}

void LevelMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    // Background
    g.setColour(DAIWLookAndFeel::Colors::surface);
    g.fillRoundedRectangle(bounds, 3.0f);

    // Calculate filled portion
    float fillAmount = displayLevel;

    // Convert to decibels for more natural display
    // -60dB to 0dB range mapped to 0-1
    if (fillAmount > 0.0f)
    {
        float db = juce::Decibels::gainToDecibels(fillAmount, -60.0f);
        fillAmount = juce::jmap(db, -60.0f, 0.0f, 0.0f, 1.0f);
    }

    if (fillAmount > 0.0f)
    {
        juce::Rectangle<float> fillBounds;

        if (vertical)
        {
            float fillHeight = bounds.getHeight() * fillAmount;
            fillBounds = bounds.removeFromBottom(fillHeight);
        }
        else
        {
            float fillWidth = bounds.getWidth() * fillAmount;
            fillBounds = bounds.removeFromLeft(fillWidth);
        }

        // Gradient from green to yellow to red
        juce::Colour fillColour;
        if (fillAmount < 0.6f)
        {
            fillColour = DAIWLookAndFeel::Colors::success;
        }
        else if (fillAmount < 0.85f)
        {
            fillColour = DAIWLookAndFeel::Colors::warning;
        }
        else
        {
            fillColour = DAIWLookAndFeel::Colors::error;
        }

        // Add glow effect for high levels
        if (fillAmount > 0.5f)
        {
            fillColour = fillColour.brighter(fillAmount - 0.5f);
        }

        g.setColour(fillColour);
        g.fillRoundedRectangle(fillBounds, 3.0f);
    }

    // Border
    g.setColour(DAIWLookAndFeel::Colors::border);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(0.5f), 3.0f, 1.0f);
}

//==============================================================================
// StereoLevelMeter
//==============================================================================

StereoLevelMeter::StereoLevelMeter()
{
    addAndMakeVisible(leftMeter);
    addAndMakeVisible(rightMeter);
    startTimerHz(30);
}

void StereoLevelMeter::setLevels(float left, float right)
{
    leftMeter.setLevel(left);
    rightMeter.setLevel(right);
}

void StereoLevelMeter::timerCallback()
{
    // Timer handled by individual meters
}

void StereoLevelMeter::paint(juce::Graphics& g)
{
    // Draw label if set
    if (label.isNotEmpty())
    {
        g.setColour(DAIWLookAndFeel::Colors::textSecondary);
        g.setFont(juce::Font(juce::FontOptions(11.0f)));
        g.drawText(label, getLocalBounds().removeFromBottom(16), juce::Justification::centred);
    }
}

void StereoLevelMeter::resized()
{
    auto bounds = getLocalBounds();

    // Reserve space for label
    if (label.isNotEmpty())
    {
        bounds.removeFromBottom(18);
    }

    auto meterWidth = (bounds.getWidth() - 4) / 2;

    leftMeter.setBounds(bounds.removeFromLeft(meterWidth));
    bounds.removeFromLeft(4); // Gap between meters
    rightMeter.setBounds(bounds.removeFromLeft(meterWidth));
}
