# DAIW Development Guide

## Development Philosophy

### 1. Ship Early, Iterate Fast

Build the smallest thing that works, then improve. A working audio passthrough is more valuable than a perfect architecture diagram.

### 2. Audio First, UI Second

The audio engine must be solid. UI can be ugly at first - it's easier to make working code pretty than to make pretty code work.

### 3. Defer Aggressively

When in doubt, don't build it yet. Every feature is debt. Add complexity only when the simpler approach fails.

### 4. Learn by Doing

JUCE is new to us. Build small experiments. Break things. Read the source code. The tutorials are good but real learning happens in the debugger.

### 5. AI-Assisted Development

We're building an AI-native DAW using AI-assisted development. Claude Code helps write the C++, we direct and review. Meta, but effective.

## Development Environment

### Required Tools

| Tool | Purpose | Install |
|------|---------|---------|
| **Xcode** | Compiler, debugger | App Store |
| **JUCE** | Framework | [juce.com/download](https://juce.com/get-juce/download) |
| **CMake** | Build system | `brew install cmake` |
| **Git** | Version control | `brew install git` |
| **Python 3.11** | AI service runtime | `brew install python@3.11` |
| **Claude Code** | AI pair programming | Already installed |

### Optional but Helpful

| Tool | Purpose |
|------|---------|
| **Surge XT** | Open source synth, good VST3 reference |
| **Pluginval** | Plugin validation tool |
| **Audacity** | Quick audio testing |

### JUCE Installation

```bash
# Download JUCE from juce.com or:
git clone https://github.com/juce-framework/JUCE.git ~/JUCE

# Set JUCE_DIR environment variable
export JUCE_DIR=~/JUCE

# Add to shell profile
echo 'export JUCE_DIR=~/JUCE' >> ~/.zshrc
```

### AI Service Setup (Python)

```bash
# Navigate to ai-service directory
cd ai-service

# Create virtual environment
python3.11 -m venv venv
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Set up environment variables
cp .env.example .env
# Edit .env to add your API keys:
#   ANTHROPIC_API_KEY=sk-ant-...
#   OPENAI_API_KEY=sk-...
#   KLANG_API_KEY=...

# Run the service (development)
uvicorn service:app --reload --port 8420

# Test the service
curl http://localhost:8420/health
```

### AI Service Dependencies

```
# requirements.txt
fastapi>=0.109.0
uvicorn>=0.27.0
httpx>=0.26.0
anthropic>=0.18.0
openai>=1.12.0
pydantic>=2.6.0
python-dotenv>=1.0.0
```

### Project Structure

```
DAIW/
├── CMakeLists.txt           # Build configuration
├── JUCE/                    # JUCE as submodule or symlink
├── Source/                  # C++ source (audio engine, UI)
│   ├── Main.cpp             # Application entry point
│   ├── MainComponent.cpp    # Main window
│   ├── Audio/
│   │   ├── AudioEngine.cpp
│   │   ├── Track.cpp
│   │   └── PluginHost.cpp
│   ├── Session/
│   │   ├── Session.cpp
│   │   ├── Clip.cpp
│   │   └── Marker.cpp
│   ├── UI/
│   │   ├── ArrangeView.cpp
│   │   ├── MixerView.cpp
│   │   └── ChatPanel.cpp
│   └── AI/
│       ├── AIClient.cpp     # HTTP client for Python service
│       └── AIController.cpp # Command execution
├── ai-service/              # Python AI service
│   ├── pyproject.toml       # Python project config
│   ├── requirements.txt     # Dependencies
│   ├── service.py           # FastAPI main entry
│   ├── providers/
│   │   ├── __init__.py
│   │   ├── anthropic.py     # Claude integration
│   │   └── openai.py        # GPT integration
│   ├── analysis/
│   │   ├── __init__.py
│   │   └── audio.py         # Audio analysis (Klang.io)
│   ├── generation/
│   │   ├── __init__.py
│   │   └── midi.py          # MIDI generation
│   └── prompts/
│       ├── system.md        # System prompt
│       └── context.md       # Context template
├── Resources/
│   └── icons/
├── Tests/
│   └── ...
├── Builds/
│   └── MacOSX/              # Xcode project (generated)
└── Docs/
    ├── VISION.md
    ├── ARCHITECTURE.md
    ├── MVP.md
    └── DEVELOPMENT.md
```

## Building the Project

### Initial Setup

```bash
# Clone the repo
git clone https://github.com/jimmypocock/DAIW.git
cd DAIW

# Generate Xcode project
mkdir build && cd build
cmake -G Xcode ..

# Open in Xcode
open DAIW.xcodeproj
```

### Build Commands

```bash
# Debug build
cmake --build build --config Debug

# Release build
cmake --build build --config Release

# Run
./build/Debug/DAIW.app/Contents/MacOS/DAIW
```

### CMakeLists.txt Template

```cmake
cmake_minimum_required(VERSION 3.22)
project(DAIW VERSION 0.1.0)

# Add JUCE
add_subdirectory(JUCE)

# Create the application
juce_add_gui_app(DAIW
    PRODUCT_NAME "DAIW"
    BUNDLE_ID "com.daiw.app"
    COMPANY_NAME "DAIW"
)

# Add source files
target_sources(DAIW PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    # ... add more as we build
)

# Link JUCE modules
target_link_libraries(DAIW PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Compile definitions
target_compile_definitions(DAIW PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
    JUCE_VST3_CAN_REPLACE_VST2=0
)
```

## Coding Standards

### Naming Conventions

```cpp
// Classes: PascalCase
class AudioEngine {};
class ChatPanel {};

// Functions: camelCase
void processAudioBlock();
void sendMessage();

// Variables: camelCase
int trackCount;
float masterVolume;

// Constants: SCREAMING_SNAKE_CASE
const int MAX_TRACKS = 128;
const float DEFAULT_TEMPO = 120.0f;

// Member variables: camelCase (no prefix)
class Track {
    juce::String name;
    float volume;
};
```

### JUCE Patterns

```cpp
// Use JUCE smart pointers
std::unique_ptr<juce::AudioPluginInstance> plugin;

// Use JUCE strings
juce::String trackName = "Vocals";

// Use JUCE containers when appropriate
juce::Array<Track*> selectedTracks;
juce::OwnedArray<Clip> clips;

// ValueTree for serialization
juce::ValueTree toValueTree() const;
static Track fromValueTree(const juce::ValueTree& v);
```

### Audio Thread Safety

```cpp
// NEVER in audio callback:
// - new / delete / malloc / free
// - Locks (mutex, critical section)
// - File I/O
// - System calls
// - String operations that allocate

// DO use:
// - Lock-free queues (juce::AbstractFifo)
// - Atomic operations
// - Pre-allocated buffers
```

### Error Handling

```cpp
// Use juce::Result for operations that can fail
juce::Result loadProject(const juce::File& file);

// Use assertions for programming errors
jassert(trackIndex >= 0 && trackIndex < tracks.size());

// Use DBG() for debug output (removed in release builds)
DBG("Loading plugin: " + pluginPath);
```

## Workflow

### Branch Strategy

```
main              # Stable releases
  └── develop     # Integration branch
       ├── feature/audio-engine
       ├── feature/mixer-view
       └── feature/ai-chat
```

### Commit Messages

```
feat: Add multi-track playback
fix: Resolve audio dropout on buffer underrun
refactor: Simplify plugin loading logic
docs: Update architecture diagram
test: Add mixer unit tests
```

### Development Cycle

1. **Pick a task** from MVP.md
2. **Create feature branch** from develop
3. **Build incrementally** - small commits
4. **Test manually** - use the app
5. **PR to develop** - review with Claude Code
6. **Merge** when stable

### Testing Strategy

**MVP approach**: Manual testing first. Automated tests for critical paths (audio engine, file I/O) when stable.

```cpp
// When we add tests, use JUCE's built-in test framework
class AudioEngineTests : public juce::UnitTest {
public:
    AudioEngineTests() : juce::UnitTest("AudioEngine") {}

    void runTest() override {
        beginTest("Buffer processing");
        // ...
    }
};
```

## Debugging

### Xcode Debugging

- Set breakpoints in audio code carefully (can cause dropouts)
- Use "Continue to here" instead of step-through in hot paths
- Profile with Instruments for performance issues

### Audio Debugging

```cpp
// Log buffer levels
DBG("Buffer RMS: " + juce::String(buffer.getRMSLevel(0, 0, buffer.getNumSamples())));

// Visualize with scope
// JUCE has AudioVisualiserComponent for this
```

### Common Issues

| Issue | Cause | Fix |
|-------|-------|-----|
| No audio output | Wrong device selected | Check AudioDeviceManager settings |
| Crackling | Buffer underrun | Increase buffer size |
| Plugin crashes | Bad plugin or threading issue | Test with known-good plugins |
| High CPU | Too many plugins or inefficient code | Profile, optimize hot paths |
| AI not responding | Python service not running | Check `curl localhost:8420/health` |
| AI service crash | Missing API keys | Check .env file has valid keys |
| Slow AI response | Network or API latency | Check API status, consider caching |
| IPC timeout | Service overloaded | Increase timeout, check service logs |

## Learning from Open Source

**Study patterns, write original code.**

Open source DAWs are invaluable learning resources. We study their architecture and design decisions, then implement our own solutions.

### How to Learn from Open Source

1. **Read the architecture**, not line-by-line code
   - How do they structure the audio engine?
   - How do they handle plugin hosting?
   - What's their threading model?

2. **Understand the patterns**
   - Model-View-Controller for UI/engine separation
   - Lock-free queues for audio thread communication
   - ValueTree for serialization (JUCE pattern)

3. **Implement your own version**
   - Use JUCE APIs directly
   - Let AI help generate implementations
   - Design for our AI-native use case

4. **Never copy-paste code**
   - Derivative work concerns are real
   - Our code should be original
   - Similar patterns ≠ copied code (patterns aren't copyrightable)

### What to Study in Each Project

| Project | Focus Areas |
|---------|-------------|
| **Tracktion Engine** | How they wrap JUCE for DAW use, Edit/Track/Clip hierarchy, plugin hosting |
| **Ardour** | Session file format, advanced routing, professional audio engine |
| **Zrythm** | Timeline UI implementation, modern UX patterns, undo system |
| **LMMS** | Beat sequencer patterns, instrument rack design |
| **Surge** | Complex plugin architecture, modulation systems |

### Example: Learning Plugin Hosting

Instead of copying Tracktion's plugin host code:

1. Read how they scan for plugins
2. Understand the lifecycle (load → process → unload)
3. Study how they handle plugin UI windows
4. Read JUCE's `AudioPluginHost` example
5. Implement our own using `juce::AudioPluginFormatManager`

The result: Same functionality, original code, no legal concerns.

---

## Bundling for Distribution

### Python Service Bundling

For release, we bundle Python inside the .app:

```bash
# Use PyInstaller to create standalone executable
cd ai-service
pip install pyinstaller

# Create single-file executable
pyinstaller --onefile --name ai-service service.py

# Or bundle with prompts
pyinstaller --onefile \
  --add-data "prompts:prompts" \
  --name ai-service \
  service.py

# Output: dist/ai-service (standalone binary)
```

### App Bundle Structure

```
DAIW.app/
└── Contents/
    ├── MacOS/
    │   └── DAIW                    # Main C++ binary
    ├── Resources/
    │   └── ai-service/
    │       ├── ai-service          # Bundled Python binary
    │       └── prompts/            # Prompt templates
    └── Info.plist
```

### CMake Integration

```cmake
# Copy ai-service to app bundle during build
add_custom_command(TARGET DAIW POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ai-service/dist
        $<TARGET_BUNDLE_DIR:DAIW>/Contents/Resources/ai-service
)
```

### Code Signing

```bash
# Sign the Python binary separately
codesign --force --deep --sign "Developer ID Application: Your Name" \
    DAIW.app/Contents/Resources/ai-service/ai-service

# Then sign the whole app
codesign --force --deep --sign "Developer ID Application: Your Name" \
    DAIW.app

# Notarize for distribution
xcrun notarytool submit DAIW.zip --apple-id ... --team-id ... --password ...
```

---

## Resources

### JUCE Documentation
- [JUCE Docs](https://docs.juce.com/)
- [JUCE Tutorials](https://juce.com/learn/tutorials)
- [JUCE Forum](https://forum.juce.com/)

### Audio Programming
- [The Audio Programmer (YouTube)](https://www.youtube.com/@TheAudioProgrammer)
- [ADC Talks](https://www.youtube.com/c/JUCElibrary)

### Open Source to Study (Architecture Only)
- [Tracktion Engine](https://github.com/Tracktion/tracktion_engine) - JUCE-based DAW engine
- [Ardour](https://github.com/Ardour/ardour) - Professional open source DAW
- [Zrythm](https://github.com/zrythm/zrythm) - Modern DAW with good UX
- [Surge](https://github.com/surge-synthesizer/surge) - Open source synth
- [JUCE Examples](https://github.com/juce-framework/JUCE/tree/master/examples) - Official examples

### Books
- "Designing Audio Effect Plugins in C++" by Will Pirkle
- "The Audio Programming Book" by Boulanger & Lazzarini

## Getting Started Checklist

### C++ / Audio Engine
- [ ] Install Xcode
- [ ] Install JUCE (download or clone)
- [ ] Install CMake via Homebrew
- [ ] Create new repo (DAIW)
- [ ] Set up CMakeLists.txt
- [ ] Build "Hello World" JUCE app
- [ ] Add audio device selector
- [ ] Achieve audio passthrough (input → output)

### Python / AI Service
- [ ] Install Python 3.11 via Homebrew
- [ ] Set up ai-service directory structure
- [ ] Create virtual environment
- [ ] Install FastAPI + dependencies
- [ ] Get Anthropic API key
- [ ] Build basic `/health` and `/process` endpoints
- [ ] Test IPC from C++ to Python

### Integration
- [ ] C++ HTTP client can reach Python service
- [ ] Send session context, receive commands
- [ ] Execute AI-generated commands in engine

Once audio passthrough AND basic AI chat work, we're off to the races.
