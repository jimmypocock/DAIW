#include "DAIWLookAndFeel.h"

DAIWLookAndFeel::DAIWLookAndFeel()
{
    // Set default colours for JUCE components
    setColour(juce::ResizableWindow::backgroundColourId, Colors::background);
    setColour(juce::DocumentWindow::backgroundColourId, Colors::background);

    // ComboBox
    setColour(juce::ComboBox::backgroundColourId, Colors::surface);
    setColour(juce::ComboBox::outlineColourId, Colors::border);
    setColour(juce::ComboBox::textColourId, Colors::textPrimary);
    setColour(juce::ComboBox::arrowColourId, Colors::textSecondary);

    // PopupMenu
    setColour(juce::PopupMenu::backgroundColourId, Colors::surface);
    setColour(juce::PopupMenu::textColourId, Colors::textPrimary);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::primaryAccent);
    setColour(juce::PopupMenu::highlightedTextColourId, Colors::background);

    // TextButton
    setColour(juce::TextButton::buttonColourId, Colors::surface);
    setColour(juce::TextButton::buttonOnColourId, Colors::primaryAccent);
    setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
    setColour(juce::TextButton::textColourOnId, Colors::background);

    // Label
    setColour(juce::Label::textColourId, Colors::textPrimary);
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);

    // ScrollBar
    setColour(juce::ScrollBar::thumbColourId, Colors::surface);
    setColour(juce::ScrollBar::trackColourId, Colors::background);
}

void DAIWLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                    bool isButtonDown, int /*buttonX*/, int /*buttonY*/,
                                    int /*buttonW*/, int /*buttonH*/, juce::ComboBox& box)
{
    auto cornerSize = 6.0f;
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);

    // Background
    auto baseColour = Colors::surface;
    if (isButtonDown)
        baseColour = Colors::surfaceHover;
    else if (box.isMouseOver())
        baseColour = Colors::surfaceHover;

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Border
    auto borderColour = box.hasKeyboardFocus(true) ? Colors::borderFocused : Colors::border;
    g.setColour(borderColour);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

    // Arrow
    auto arrowZone =
        juce::Rectangle<int>(width - 30, 0, 20, height).toFloat().reduced(8.0f, 10.0f);

    juce::Path arrow;
    arrow.addTriangle(arrowZone.getX(), arrowZone.getY(),
                      arrowZone.getCentreX(), arrowZone.getBottom(),
                      arrowZone.getRight(), arrowZone.getY());

    g.setColour(Colors::textSecondary);
    g.fillPath(arrow);
}

void DAIWLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();

    g.setColour(Colors::surface);
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(Colors::border);
    g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);
}

void DAIWLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                         bool isSeparator, bool isActive, bool isHighlighted,
                                         bool /*isTicked*/, bool /*hasSubMenu*/,
                                         const juce::String& text,
                                         const juce::String& /*shortcutKeyText*/,
                                         const juce::Drawable* /*icon*/,
                                         const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        auto r = area.reduced(5, 0);
        r.removeFromTop(juce::roundToInt((r.getHeight() * 0.5f) - 0.5f));

        g.setColour(Colors::border);
        g.fillRect(r.removeFromTop(1));
        return;
    }

    auto textColourToUse = Colors::textPrimary;

    if (isHighlighted && isActive)
    {
        g.setColour(Colors::primaryAccent.withAlpha(0.2f));
        g.fillRoundedRectangle(area.toFloat().reduced(2.0f), 4.0f);
        textColourToUse = Colors::primaryAccent;
    }

    g.setColour(isActive ? textColourToUse : Colors::textMuted);
    g.setFont(getPopupMenuFont());

    auto textArea = area.reduced(12, 0);
    g.drawFittedText(text, textArea, juce::Justification::centredLeft, 1);
}

void DAIWLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& /*backgroundColour*/,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
    auto cornerSize = 6.0f;

    juce::Colour baseColour;

    if (button.getToggleState())
    {
        baseColour = Colors::primaryAccent;
    }
    else if (shouldDrawButtonAsDown)
    {
        baseColour = Colors::surfaceHover;
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        baseColour = Colors::surfaceHover;
    }
    else
    {
        baseColour = Colors::surface;
    }

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Border for non-toggled buttons
    if (!button.getToggleState())
    {
        g.setColour(Colors::border);
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    }
}

void DAIWLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());

        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(getLabelFont(label));
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         juce::jmax(1, (int)(textArea.getHeight() / g.getCurrentFont().getHeight())),
                         label.getMinimumHorizontalScale());
    }
}

void DAIWLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y,
                                     int width, int height, bool isScrollbarVertical,
                                     int thumbStartPosition, int thumbSize, bool isMouseOver,
                                     bool isMouseDown)
{
    juce::ignoreUnused(scrollbar);

    auto thumbColour = Colors::surface;
    if (isMouseDown)
        thumbColour = Colors::primaryAccent;
    else if (isMouseOver)
        thumbColour = Colors::surfaceHover;

    juce::Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
    {
        thumbBounds = juce::Rectangle<int>(x + 2, thumbStartPosition, width - 4, thumbSize);
    }
    else
    {
        thumbBounds = juce::Rectangle<int>(thumbStartPosition, y + 2, thumbSize, height - 4);
    }

    g.setColour(thumbColour);
    g.fillRoundedRectangle(thumbBounds.toFloat(), 4.0f);
}

juce::Font DAIWLookAndFeel::getLabelFont(juce::Label& label)
{
    return juce::Font(juce::FontOptions(label.getFont().getHeight()));
}

juce::Font DAIWLookAndFeel::getComboBoxFont(juce::ComboBox& /*box*/)
{
    return juce::Font(juce::FontOptions(15.0f));
}

juce::Font DAIWLookAndFeel::getPopupMenuFont()
{
    return juce::Font(juce::FontOptions(15.0f));
}
