#include "MainComponent.h"

class MainWindow : public juce::DocumentWindow, public juce::MenuBarModel
{
public:
    MainWindow(juce::String name)
        : DocumentWindow(
              name,
              juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                  juce::ResizableWindow::backgroundColourId),
              DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);

        mainComponent = new MainComponent();
        setContentOwned(mainComponent, true);

        // Setup menu bar
        setMenuBar(this);

        setResizable(true, true);
        centreWithSize(1280, 800);
        setVisible(true);

        // Add keyboard listener for commands
        addKeyListener(mainComponent->getCommandManager().getKeyMappings());
    }

    ~MainWindow() override
    {
        setMenuBar(nullptr);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

    // Menu bar
    juce::StringArray getMenuBarNames() override
    {
        return {"DAIW", "Edit", "View"};
    }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& /*menuName*/) override
    {
        juce::PopupMenu menu;
        auto& commandManager = mainComponent->getCommandManager();

        if (menuIndex == 0) // DAIW menu
        {
            menu.addCommandItem(&commandManager, MainComponent::openSettings);
            menu.addSeparator();
            menu.addItem(1, "Quit DAIW", true, false, nullptr);
        }
        else if (menuIndex == 1) // Edit menu
        {
            menu.addItem(2, "Undo", false);
            menu.addItem(3, "Redo", false);
        }
        else if (menuIndex == 2) // View menu
        {
            menu.addItem(4, "Zoom In", false);
            menu.addItem(5, "Zoom Out", false);
        }

        return menu;
    }

    void menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) override
    {
        if (menuItemID == 1)
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }

private:
    MainComponent* mainComponent = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

class DAIWApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override
    {
        return "DAIW";
    }

    const juce::String getApplicationVersion() override
    {
        return "0.1.0";
    }

    bool moreThanOneInstanceAllowed() override
    {
        return false;
    }

    void initialise(const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(DAIWApplication)
