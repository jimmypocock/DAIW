#include "SettingsWindow.h"
#include "../Audio/AudioEngine.h"

//==============================================================================
// SidebarButton
//==============================================================================

SettingsWindow::SidebarButton::SidebarButton(const juce::String& name) : juce::TextButton(name)
{
    setClickingTogglesState(true);
    setRadioGroupId(1);
}

void SettingsWindow::SidebarButton::paintButton(juce::Graphics& g,
                                                 bool shouldDrawButtonAsHighlighted,
                                                 bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = getLocalBounds().toFloat();
    auto isSelected = getToggleState();

    // Highlight background
    if (isSelected)
    {
        g.setColour(DAIWLookAndFeel::Colors::primaryAccent.withAlpha(0.15f));
        g.fillRoundedRectangle(bounds.reduced(4.0f, 2.0f), 6.0f);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(DAIWLookAndFeel::Colors::surface);
        g.fillRoundedRectangle(bounds.reduced(4.0f, 2.0f), 6.0f);
    }

    // Left accent bar for selected
    if (isSelected)
    {
        g.setColour(DAIWLookAndFeel::Colors::primaryAccent);
        g.fillRoundedRectangle(bounds.removeFromLeft(3.0f).reduced(0.0f, 8.0f), 1.5f);
    }

    // Text
    g.setColour(isSelected ? DAIWLookAndFeel::Colors::primaryAccent
                           : DAIWLookAndFeel::Colors::textSecondary);
    g.setFont(juce::Font(juce::FontOptions(14.0f)));
    g.drawText(getButtonText(), getLocalBounds().reduced(16, 0), juce::Justification::centredLeft);
}

//==============================================================================
// SettingsWindow
//==============================================================================

SettingsWindow::SettingsWindow(AudioEngine& engine) : audioEngine(engine)
{
    setVisible(false);
    setWantsKeyboardFocus(true);

    // Title
    titleLabel.setText("Settings", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions(20.0f).withStyle("Bold")));
    titleLabel.setColour(juce::Label::textColourId, DAIWLookAndFeel::Colors::textPrimary);
    addAndMakeVisible(titleLabel);

    // Close button
    closeButton.setButtonText(juce::String::charToString(0x2715)); // ✕
    closeButton.onClick = [this]() { hide(); };
    addAndMakeVisible(closeButton);

    // Sidebar buttons
    auto audioButton = std::make_unique<SidebarButton>("Audio");
    audioButton->setToggleState(true, juce::dontSendNotification);
    audioButton->onClick = [this]() { selectPanel(0); };
    addAndMakeVisible(*audioButton);
    sidebarButtons.push_back(std::move(audioButton));

    auto aiButton = std::make_unique<SidebarButton>("AI");
    aiButton->onClick = [this]() { selectPanel(1); };
    aiButton->setEnabled(false); // Coming soon
    addAndMakeVisible(*aiButton);
    sidebarButtons.push_back(std::move(aiButton));

    auto generalButton = std::make_unique<SidebarButton>("General");
    generalButton->onClick = [this]() { selectPanel(2); };
    generalButton->setEnabled(false); // Coming soon
    addAndMakeVisible(*generalButton);
    sidebarButtons.push_back(std::move(generalButton));

    // Audio settings panel
    audioSettingsPanel = std::make_unique<AudioSettingsPanel>(audioEngine);
    addAndMakeVisible(*audioSettingsPanel);
}

juce::Rectangle<int> SettingsWindow::getPanelBounds() const
{
    return getLocalBounds().withSizeKeepingCentre(panelWidth, panelHeight);
}

void SettingsWindow::paint(juce::Graphics& g)
{
    // Semi-transparent backdrop
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.fillRect(getLocalBounds());

    auto panelBounds = getPanelBounds().toFloat();

    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(panelBounds.translated(0.0f, 8.0f), 12.0f);

    // Panel background
    g.setColour(DAIWLookAndFeel::Colors::panelBackground);
    g.fillRoundedRectangle(panelBounds, 12.0f);

    // Sidebar background
    auto sidebarBounds = panelBounds.removeFromLeft(sidebarWidth);

    // Create a path for rounded left corners only
    juce::Path sidebarPath;
    sidebarPath.addRoundedRectangle(sidebarBounds.getX(), sidebarBounds.getY(),
                                     sidebarBounds.getWidth() + 12.0f, sidebarBounds.getHeight(),
                                     12.0f, 12.0f, true, false, true, false);

    g.setColour(DAIWLookAndFeel::Colors::background);
    g.fillPath(sidebarPath);

    // Divider line
    g.setColour(DAIWLookAndFeel::Colors::border);
    auto dividerX = static_cast<float>(getPanelBounds().getX() + sidebarWidth);
    g.drawVerticalLine(static_cast<int>(dividerX),
                       static_cast<float>(getPanelBounds().getY()),
                       static_cast<float>(getPanelBounds().getBottom()));
}

void SettingsWindow::resized()
{
    auto panelBounds = getPanelBounds();

    // Header area within panel
    auto headerBounds = panelBounds.removeFromTop(headerHeight);

    // Title in sidebar area
    auto titleBounds = headerBounds.removeFromLeft(sidebarWidth).reduced(padding, 0);
    titleLabel.setBounds(titleBounds);

    // Close button in top right
    closeButton.setBounds(headerBounds.removeFromRight(50).reduced(10, 10));

    // Sidebar within panel
    auto sidebarBounds = panelBounds.removeFromLeft(sidebarWidth);
    sidebarBounds.removeFromTop(10);

    for (auto& button : sidebarButtons)
    {
        button->setBounds(sidebarBounds.removeFromTop(36));
    }

    // Content area within panel
    auto contentBounds = panelBounds.reduced(padding);

    if (audioSettingsPanel)
    {
        audioSettingsPanel->setBounds(contentBounds);
    }
}

void SettingsWindow::mouseDown(const juce::MouseEvent& event)
{
    // Close if clicking outside the panel
    if (!getPanelBounds().contains(event.getPosition()))
    {
        hide();
    }
}

bool SettingsWindow::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        hide();
        return true;
    }
    return false;
}

void SettingsWindow::selectPanel(int index)
{
    selectedPanelIndex = index;

    // Show/hide panels based on selection
    if (audioSettingsPanel)
    {
        audioSettingsPanel->setVisible(index == 0);
    }

    // Future panels would be shown/hidden here
}

void SettingsWindow::show()
{
    setVisible(true);

    if (auto* parent = getParentComponent())
    {
        setBounds(parent->getLocalBounds());
        toFront(true);
        grabKeyboardFocus();
    }
}

void SettingsWindow::hide()
{
    setVisible(false);

    if (auto* parent = getParentComponent())
    {
        parent->repaint();
    }
}
