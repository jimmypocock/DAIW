# Getting Started with DAIW Development

> **Prerequisites**: macOS, basic familiarity with C++ and Python
> **Time to Setup**: ~30 minutes

## Development Environment

### Required Tools

#### 1. Xcode (C++ Compiler & IDE)

```bash
# Install Xcode from App Store, then run:
xcode-select --install
```

This gives you:
- C++ compiler (clang)
- macOS SDK
- Instruments for profiling
- Code signing tools for distribution

#### 2. JUCE Framework

JUCE is the industry-standard framework for audio applications (used by Ableton, Native Instruments, ROLI).

```bash
# Option A: Download from website
# https://juce.com/download/

# Option B: Clone from GitHub (recommended for version control)
git clone https://github.com/juce-framework/JUCE.git ~/JUCE
```

JUCE includes:
- **Projucer** - GUI tool for creating/managing JUCE projects
- **Audio plugin hosting** - VST3/AU support built-in
- **Example projects** - Great for learning patterns

**License**: Free for GPL projects, $40/month for commercial (>$50k revenue)

#### 3. CMake (Build System)

```bash
brew install cmake
```

CMake 3.22+ required for JUCE's CMake integration.

#### 4. Python (for AI Service)

```bash
# Python 3.11+ recommended
brew install python@3.11

# Or use pyenv for version management
brew install pyenv
pyenv install 3.11.8
pyenv global 3.11.8
```

#### 5. Homebrew (if not installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Optional but Helpful

```bash
# Audio manipulation CLI
brew install sox

# Format conversion
brew install ffmpeg

# Plugin validation tool
brew install --cask pluginval

# Code editor (if not using Xcode)
brew install --cask visual-studio-code
```

---

## Project Structure

```
DAIW/
├── CMakeLists.txt              # Root CMake configuration
├── JUCE/                       # JUCE framework (submodule)
├── src/
│   ├── Main.cpp                # Application entry point
│   ├── MainComponent.cpp       # Main window component
│   ├── MainComponent.h
│   ├── Audio/
│   │   ├── AudioEngine.cpp     # Core audio processing
│   │   ├── AudioEngine.h
│   │   ├── Track.cpp
│   │   ├── Track.h
│   │   ├── Clip.cpp
│   │   └── Clip.h
│   ├── Session/
│   │   ├── Session.cpp         # Project state management
│   │   ├── Session.h
│   │   └── Transport.h
│   ├── Plugins/
│   │   ├── PluginHost.cpp      # VST3/AU hosting
│   │   └── PluginHost.h
│   ├── UI/
│   │   ├── ArrangementView.cpp # Timeline/arrangement
│   │   ├── ArrangementView.h
│   │   ├── MixerView.cpp       # Mixer panel
│   │   ├── MixerView.h
│   │   ├── ChatPanel.cpp       # AI chat interface
│   │   └── ChatPanel.h
│   └── AI/
│       ├── AIBridge.cpp        # C++ HTTP client for Python service
│       └── AIBridge.h
├── ai-service/                 # Python AI service (bundled in app)
│   ├── requirements.txt
│   ├── service.py              # FastAPI server
│   ├── orchestrator.py         # AI orchestration logic
│   ├── analysis.py             # Audio analysis
│   └── providers/
│       ├── __init__.py
│       ├── anthropic.py        # Claude client
│       ├── openai.py           # OpenAI client
│       └── elevenlabs.py       # ElevenLabs music generation
├── resources/
│   ├── prompts/
│   │   ├── system.md           # System prompt
│   │   └── skills/
│   └── icons/
├── tests/
│   ├── AudioEngineTests.cpp
│   └── ...
├── docs/                       # Documentation (port planning docs here)
│   ├── ARCHITECTURE.md
│   ├── MVP.md
│   ├── VISION.md
│   └── ...
├── .gitignore
├── README.md
└── LICENSE
```

---

## Quick Start: Hello World

### 1. Create Project Directory

```bash
mkdir -p ~/Projects/DAIW && cd ~/Projects/DAIW
git init
```

### 2. Add JUCE

```bash
# As a submodule (recommended)
git submodule add https://github.com/juce-framework/JUCE.git JUCE

# Or just clone it
git clone https://github.com/juce-framework/JUCE.git JUCE
```

### 3. Create CMakeLists.txt

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(DAIW VERSION 0.1.0)

# Add JUCE
add_subdirectory(JUCE)

# Create the application
juce_add_gui_app(DAIW
    PRODUCT_NAME "DAIW"
    COMPANY_NAME "YourCompany"
    BUNDLE_ID "com.yourcompany.daiw"
    ICON_BIG ""
    ICON_SMALL ""
)

# Source files
target_sources(DAIW PRIVATE
    src/Main.cpp
    src/MainComponent.cpp
)

# Link JUCE modules
target_link_libraries(DAIW PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Compile definitions
target_compile_definitions(DAIW PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:DAIW,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:DAIW,JUCE_VERSION>"
)

# C++ standard
target_compile_features(DAIW PRIVATE cxx_std_17)
```

### 4. Create Main.cpp

```cpp
// src/Main.cpp
#include "MainComponent.h"

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(juce::String name)
        : DocumentWindow(name,
            juce::Desktop::getInstance().getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
            DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(), true);
        setResizable(true, true);
        centreWithSize(1280, 800);
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

class DAIWApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "DAIW"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

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
```

### 5. Create MainComponent.h

```cpp
// src/MainComponent.h
#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
```

### 6. Create MainComponent.cpp

```cpp
// src/MainComponent.cpp
#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1280, 800);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xff1e1e1e));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("DAIW", getLocalBounds(), juce::Justification::centred);

    // Subtitle
    g.setFont(juce::Font(16.0f));
    g.setColour(juce::Colours::grey);
    g.drawText("Digital AI Workstation - Coming Soon",
               getLocalBounds().translated(0, 50),
               juce::Justification::centred);
}

void MainComponent::resized()
{
    // Layout child components here
}
```

### 7. Create Directory Structure

```bash
mkdir -p src
# Copy the files above into src/
```

### 8. Build and Run

```bash
# Create build directory
mkdir build && cd build

# Generate Xcode project (or Makefiles)
cmake .. -G Xcode
# Or for command line: cmake ..

# Build
cmake --build . --config Debug

# Run (macOS)
open DAIW_artefacts/Debug/DAIW.app

# Or from command line build:
./DAIW_artefacts/Debug/DAIW
```

---

## Verification Checklist

Run these commands to verify your environment is set up correctly:

```bash
# Xcode command line tools
xcode-select -p
# Expected: /Applications/Xcode.app/Contents/Developer

# CMake version
cmake --version
# Expected: 3.22+

# Python version
python3 --version
# Expected: 3.11+

# Homebrew
brew --version
# Expected: Any version

# Check audio devices (sanity check)
system_profiler SPAudioDataType | head -20
# Expected: List of audio devices
```

---

## Learning Path

Before building the full DAW, work through these milestones in order:

| Phase | Build This | What You Learn |
|-------|-----------|----------------|
| **1** | Audio passthrough (input → output) | Audio I/O, device selection, buffer callbacks |
| **2** | Play a WAV file | Audio formats, file I/O, transport |
| **3** | Multiple tracks mixing | Buffer management, gain staging, summing |
| **4** | Basic recording | Input monitoring, file writing |
| **5** | Timeline UI with waveforms | Custom components, graphics, scrolling |
| **6** | Load and host a VST3 plugin | Plugin hosting, parameter control |
| **7** | MIDI playback | MIDI sequencing, timing |
| **8** | Python service integration | IPC, HTTP client, subprocess management |

### Recommended JUCE Tutorials

1. [Getting Started](https://juce.com/learn/tutorials/getting-started)
2. [Build an Audio Player](https://juce.com/learn/tutorials/build-an-audio-player)
3. [Recording Audio](https://juce.com/learn/tutorials/recording-audio)
4. [Hosting Audio Plugins](https://juce.com/learn/tutorials/hosting-audio-plugins)

### Study These Projects

| Project | What to Learn |
|---------|---------------|
| [JUCE AudioPluginHost](https://github.com/juce-framework/JUCE/tree/master/extras/AudioPluginHost) | Plugin hosting patterns |
| [Tracktion Engine](https://github.com/Tracktion/tracktion_engine) | DAW architecture (study, don't copy) |
| [Helio Workstation](https://github.com/helio-fm/helio-workstation) | Modern DAW UI patterns |

---

## Python AI Service Setup

Once the C++ side is working, set up the AI service:

```bash
# Create virtual environment
cd ~/Projects/DAIW/ai-service
python3 -m venv venv
source venv/bin/activate

# Install dependencies
pip install fastapi uvicorn anthropic openai requests

# Create requirements.txt
pip freeze > requirements.txt
```

### Minimal service.py

```python
# ai-service/service.py
from fastapi import FastAPI
import uvicorn

app = FastAPI()

@app.get("/health")
def health():
    return {"status": "ok", "version": "0.1.0"}

@app.post("/process")
def process(request: dict):
    # TODO: Implement AI orchestration
    return {
        "response": "AI service is running",
        "commands": []
    }

if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8420)
```

### Test the Service

```bash
# Terminal 1: Run service
python service.py

# Terminal 2: Test it
curl http://localhost:8420/health
# {"status":"ok","version":"0.1.0"}
```

---

## .gitignore

```gitignore
# Build artifacts
build/
cmake-build-*/
*.xcodeproj/
*.xcworkspace/

# JUCE generated
Builds/
JuceLibraryCode/

# Python
__pycache__/
*.pyc
venv/
.venv/
*.egg-info/

# IDE
.idea/
.vscode/
*.swp
*.swo
.DS_Store

# Secrets
.env
config.json

# Dependencies (if not using submodules)
# JUCE/
```

---

## Troubleshooting

### CMake can't find JUCE

```bash
# Make sure JUCE is in the right place
ls -la JUCE/
# Should see CMakeLists.txt, modules/, etc.
```

### Audio device access denied

```bash
# Check System Preferences → Security & Privacy → Microphone
# Ensure your terminal/IDE has microphone access
```

### Build errors about missing headers

```bash
# Clean and rebuild
rm -rf build/
mkdir build && cd build
cmake ..
cmake --build .
```

### Plugin validation fails

```bash
# Run pluginval to check VST3 plugins
pluginval --validate /path/to/plugin.vst3
```

---

## Next Steps

1. Get the hello world app building and running
2. Work through JUCE audio tutorials
3. Implement audio passthrough (Phase 1)
4. Port the planning docs to the new project's `docs/` folder
5. Start building toward MVP milestones

---

## Resources

- [JUCE Documentation](https://juce.com/learn/documentation)
- [JUCE Forum](https://forum.juce.com/)
- [The Audio Programmer YouTube](https://www.youtube.com/@TheAudioProgrammer)
- [JUCE GitHub](https://github.com/juce-framework/JUCE)
