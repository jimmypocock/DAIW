#pragma once

#include <JuceHeader.h>

/**
 * DAIWLookAndFeel provides the custom visual styling for the application.
 *
 * Design: Dark synthwave aesthetic with cyan/purple accents.
 * Accessible: High contrast, readable text, clear interactive states.
 */
class DAIWLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Color palette
    struct Colors
    {
        // Backgrounds
        static inline const juce::Colour background{0xff0f0f1a};
        static inline const juce::Colour panelBackground{0xff1a1a2e};
        static inline const juce::Colour surface{0xff252542};
        static inline const juce::Colour surfaceHover{0xff2f2f52};

        // Accents
        static inline const juce::Colour primaryAccent{0xff00d4ff};   // Cyan
        static inline const juce::Colour secondaryAccent{0xffbf5af2}; // Purple

        // Text
        static inline const juce::Colour textPrimary{0xffffffff};
        static inline const juce::Colour textSecondary{0xff8888aa};
        static inline const juce::Colour textMuted{0xff555577};

        // Borders
        static inline const juce::Colour border{0xff333355};
        static inline const juce::Colour borderFocused{0xff00d4ff};

        // Status
        static inline const juce::Colour success{0xff32d583};
        static inline const juce::Colour warning{0xffffb347};
        static inline const juce::Colour error{0xffff6b6b};
    };

    DAIWLookAndFeel();
    ~DAIWLookAndFeel() override = default;

    // ComboBox styling
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText, const juce::Drawable* icon,
                           const juce::Colour* textColour) override;

    // Button styling
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    // Label styling
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    // Scrollbar styling
    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y,
                       int width, int height, bool isScrollbarVertical,
                       int thumbStartPosition, int thumbSize, bool isMouseOver,
                       bool isMouseDown) override;

    // Font
    juce::Font getLabelFont(juce::Label& label) override;
    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    juce::Font getPopupMenuFont() override;

private:
    juce::Typeface::Ptr typeface;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAIWLookAndFeel)
};
