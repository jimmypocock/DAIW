# DAIW Architecture

## Build Philosophy

**We build from scratch, learn from others.**

There are several open source DAWs and engines we can study for architecture patterns and design decisions:

| Project | What to Learn |
|---------|---------------|
| [Tracktion Engine](https://github.com/Tracktion/tracktion_engine) | JUCE-based DAW architecture, plugin hosting patterns |
| [Ardour](https://github.com/Ardour/ardour) | Professional audio engine design, session management |
| [Zrythm](https://github.com/zrythm/zrythm) | Modern DAW UX patterns, timeline implementation |
| [LMMS](https://github.com/LMMS/lmms) | Qt-based audio UI, MIDI sequencing |

**Important**: We study these for *patterns and concepts*, not code. We implement our own solutions using JUCE APIs and AI assistance. This ensures:
- Full ownership of our codebase
- No licensing complications or derivative work concerns
- Freedom to commercialize however we choose
- Architecture tailored to our AI-native vision

With AI-assisted development, we can move fast while building clean, original code.

---

## High-Level Architecture

DAIW uses a **two-process architecture**: a C++/JUCE application for audio and UI, and a bundled Python service for AI orchestration.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              DAIW.app Bundle                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────────────────────────────────────────────────────────┐  │
│   │                    C++ / JUCE (Main Process)                        │  │
│   │                                                                     │  │
│   │   ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐   │  │
│   │   │  Audio      │  │   Plugin    │  │      UI Layer           │   │  │
│   │   │  Engine     │  │   Host      │  │  (Arrangement, Mixer,   │   │  │
│   │   │             │  │  (VST3/AU)  │  │   Chat Panel)           │   │  │
│   │   └─────────────┘  └─────────────┘  └─────────────────────────┘   │  │
│   │                                                                     │  │
│   └───────────────────────────────┬─────────────────────────────────────┘  │
│                                   │                                         │
│                          Local HTTP (localhost:8420)                        │
│                                   │                                         │
│   ┌───────────────────────────────┴─────────────────────────────────────┐  │
│   │                    Python (AI Service Process)                       │  │
│   │                                                                     │  │
│   │   ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐   │  │
│   │   │   Audio     │  │     AI      │  │     LLM Client          │   │  │
│   │   │  Analysis   │  │ Orchestrator│  │  (Claude/OpenAI/Ollama) │   │  │
│   │   └─────────────┘  └─────────────┘  └─────────────────────────┘   │  │
│   │                                                                     │  │
│   └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
                    │                                   │
                    ▼                                   ▼
        ┌───────────────────┐               ┌───────────────────────┐
        │   Audio Interface │               │   External APIs       │
        │      (SSL 2)      │               │  (Claude, Klang.io)   │
        └───────────────────┘               └───────────────────────┘
```

### Why Two Processes?

| Concern | C++ (JUCE) | Python |
|---------|------------|--------|
| Real-time audio | ✅ Required | ❌ GC pauses |
| Plugin hosting | ✅ VST3 SDK is C++ | ❌ Not practical |
| AI orchestration | ❌ Verbose, slow iteration | ✅ LangChain, easy APIs |
| HTTP/API calls | ❌ Complex (libcurl) | ✅ Native (requests) |
| Rapid iteration | ❌ Compile times | ✅ Instant changes |

**Best of both worlds**: C++ handles what it's good at (audio), Python handles what it's good at (AI).

---

## Deployment Model

### Single App Bundle

Users download one file: `DAIW.app` (~200-500MB)

```
DAIW.app/
├── Contents/
│   ├── MacOS/
│   │   └── DAIW                          # Main C++ executable
│   ├── Frameworks/
│   │   └── ...                           # JUCE frameworks
│   └── Resources/
│       ├── ai-service/                   # Bundled Python environment
│       │   ├── python3.11                # Python runtime (standalone)
│       │   ├── venv/
│       │   │   └── site-packages/        # Dependencies
│       │   │       ├── anthropic/
│       │   │       ├── openai/
│       │   │       ├── requests/
│       │   │       ├── langchain/        # Optional, for complex orchestration
│       │   │       └── essentia/         # Local audio analysis (offline)
│       │   ├── service.py                # Main AI service
│       │   ├── analysis.py               # Audio analysis module
│       │   ├── orchestrator.py           # AI orchestration logic
│       │   └── prompts/                  # System prompts, skills
│       │       ├── system.md
│       │       ├── drums.md
│       │       └── mixing.md
│       └── default-plugins/              # Built-in effects (optional)
└── Info.plist
```

### Startup Sequence

```
1. User double-clicks DAIW.app
            ↓
2. macOS launches C++ binary (Contents/MacOS/DAIW)
            ↓
3. C++ app spawns Python subprocess:
   Contents/Resources/ai-service/python3.11 service.py
            ↓
4. Python service starts HTTP server on localhost:8420
            ↓
5. C++ app waits for health check response
            ↓
6. App is ready - user sees main window
```

### What Runs Where

| Component | Location | Internet Required |
|-----------|----------|-------------------|
| Audio engine | Local (C++) | No |
| UI rendering | Local (C++) | No |
| VST3 plugins | Local (user's plugins) | No |
| Recording/playback | Local (C++) | No |
| AI orchestration | Local (Python) | No |
| LLM (cloud) | Claude/OpenAI servers | Yes |
| LLM (offline) | Local Ollama | No |
| Audio analysis (cloud) | Klang.io servers | Yes |
| Audio analysis (offline) | Local Essentia | No |

**We run zero servers.** Everything is local or direct API calls with user's keys.

---

## Offline Mode

DAIW works offline with reduced AI capabilities:

### Online vs Offline

| Feature | Online | Offline |
|---------|--------|---------|
| Recording | ✅ | ✅ |
| Playback | ✅ | ✅ |
| Mixing | ✅ | ✅ |
| VST Plugins | ✅ | ✅ |
| Manual editing | ✅ | ✅ |
| AI (Claude/GPT-4) | ✅ Best quality | ❌ |
| AI (Ollama local) | ✅ | ✅ Slower, less capable |
| Audio analysis (Klang.io) | ✅ Best accuracy | ❌ |
| Audio analysis (Essentia) | ✅ | ✅ Good accuracy |

### Offline Architecture

```python
# In Python service - automatic fallback
class AIClient:
    def __init__(self, config):
        self.online = self._check_connectivity()

    def get_llm(self):
        if self.online and config.api_key:
            return ClaudeClient(api_key=config.api_key)
        else:
            return OllamaClient(model="llama3.2")  # Local

    def analyze_audio(self, path):
        if self.online and config.analysis_api_key:
            return klang_api.analyze(path)  # Cloud
        else:
            return essentia_analyze(path)    # Local, bundled
```

---

## Tech Stack Details

### C++ / JUCE Layer

**Framework**: [JUCE](https://juce.com/) 7.x

**Why JUCE:**
- Industry standard (Ableton, Native Instruments, ROLI)
- Built-in VST3/AU plugin hosting
- Cross-platform audio I/O (CoreAudio, ASIO, WASAPI)
- Mature, battle-tested, well-documented
- Handles audio threading, buffer management, sample rate conversion

**License:**
- Free for GPL projects
- Commercial: $40/month (for >$50k revenue)

**Build System:**
- CMake + JUCE's CMake integration
- Xcode for macOS development

### Python AI Service Layer

**Runtime**: Python 3.11+ (bundled with app)

**Key Dependencies:**

| Package | Purpose |
|---------|---------|
| `anthropic` | Claude API client |
| `openai` | OpenAI API client |
| `requests` | HTTP calls to analysis APIs |
| `flask` or `fastapi` | Local HTTP server |
| `essentia` | Offline audio analysis |
| `langchain` (optional) | Complex AI orchestration |

**Bundling**: Use `pyinstaller` or custom venv bundling during build.

---

## AI Orchestration Flow

### The Core Loop

When a user says "Add drums that match my recording":

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ 1. USER INPUT                                                               │
│    "Add drums that match my guitar recording"                               │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    ↓
┌─────────────────────────────────────────────────────────────────────────────┐
│ 2. C++ APP                                                                  │
│    - Captures selected track info (name, plugins, position)                 │
│    - Exports audio region to temp file                                      │
│    - Sends to Python service via HTTP POST                                  │
│                                                                             │
│    POST http://localhost:8420/process                                       │
│    {                                                                        │
│      "action": "generate_accompaniment",                                    │
│      "audio_path": "/tmp/daiw_audio_12345.wav",                            │
│      "user_message": "Add drums that match my guitar recording",           │
│      "context": {                                                           │
│        "selected_track": "Guitar",                                          │
│        "tempo": 120,                                                        │
│        "time_signature": "4/4",                                             │
│        "position": 0.0                                                      │
│      }                                                                      │
│    }                                                                        │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    ↓
┌─────────────────────────────────────────────────────────────────────────────┐
│ 3. PYTHON SERVICE - Audio Analysis                                          │
│                                                                             │
│    analysis = analyze_audio("/tmp/daiw_audio_12345.wav")                   │
│    # Returns:                                                               │
│    # {                                                                      │
│    #   "bpm": 95,                                                          │
│    #   "key": "G major",                                                   │
│    #   "chords": ["G", "C", "D", "Em"],                                    │
│    #   "sections": [{"start": 0, "end": 16, "type": "verse"}],             │
│    #   "mood": "folk, acoustic, mellow"                                    │
│    # }                                                                      │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    ↓
┌─────────────────────────────────────────────────────────────────────────────┐
│ 4. PYTHON SERVICE - LLM Call                                                │
│                                                                             │
│    prompt = f"""                                                            │
│    You are DAIW, an AI music assistant.                                     │
│                                                                             │
│    User's recording analysis:                                               │
│    - BPM: {analysis.bpm}                                                   │
│    - Key: {analysis.key}                                                   │
│    - Chords: {analysis.chords}                                             │
│    - Feel: {analysis.mood}                                                 │
│                                                                             │
│    User request: Add drums that match my guitar recording                   │
│                                                                             │
│    Generate a drum pattern as MIDI data that complements this recording.    │
│    Output JSON with notes in format: {note, velocity, start_beat, duration} │
│    """                                                                      │
│                                                                             │
│    response = claude.messages.create(...)                                   │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    ↓
┌─────────────────────────────────────────────────────────────────────────────┐
│ 5. PYTHON SERVICE - Parse Response                                          │
│                                                                             │
│    # LLM returns:                                                           │
│    {                                                                        │
│      "response": "I've created a folk-style brush drum pattern...",        │
│      "commands": [{                                                         │
│        "action": "create_midi_track",                                       │
│        "name": "Drums",                                                     │
│        "plugin": "drums"                                                    │
│      }, {                                                                   │
│        "action": "add_midi_notes",                                          │
│        "track": "Drums",                                                    │
│        "notes": [                                                           │
│          {"note": 36, "velocity": 80, "start": 0.0, "duration": 0.5},      │
│          {"note": 38, "velocity": 70, "start": 1.0, "duration": 0.25},     │
│          ...                                                                │
│        ]                                                                    │
│      }]                                                                     │
│    }                                                                        │
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    ↓
┌─────────────────────────────────────────────────────────────────────────────┐
│ 6. C++ APP - Execute Commands                                               │
│                                                                             │
│    for (auto& cmd : response.commands) {                                    │
│        if (cmd.action == "create_midi_track")                               │
│            session.createTrack(cmd.name, TrackType::MIDI);                  │
│        else if (cmd.action == "add_midi_notes")                             │
│            session.getTrack(cmd.track)->addNotes(cmd.notes);                │
│    }                                                                        │
│                                                                             │
│    // Update UI, show response in chat panel                                │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Inter-Process Communication (IPC)

### Protocol: Local HTTP

Python service runs a simple HTTP server on `localhost:8420`.

**Why HTTP over sockets/pipes:**
- Simple to implement and debug
- JSON payloads are natural for both sides
- Can test Python service independently (curl, Postman)
- Easy to add more endpoints

### API Endpoints

```
POST /process
    Main endpoint for AI requests
    Body: { action, audio_path?, user_message, context }
    Response: { response, commands[], error? }

POST /analyze
    Analyze audio file
    Body: { audio_path }
    Response: { bpm, key, chords, sections, mood }

GET /health
    Health check
    Response: { status: "ok", version: "0.1.0" }

POST /config
    Update configuration (API keys, model selection)
    Body: { api_key?, model?, offline_mode? }
    Response: { status: "ok" }
```

### C++ HTTP Client

Using JUCE's `URL` class or a lightweight library:

```cpp
void AIClient::sendRequest(const juce::var& payload,
                           std::function<void(juce::var)> callback) {
    juce::URL url("http://localhost:8420/process");

    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders("Content-Type: application/json")
        .withPostData(juce::JSON::toString(payload));

    // Run on background thread
    threadPool.addJob([=]() {
        auto stream = url.createInputStream(options);
        auto response = stream->readEntireStreamAsString();
        auto json = juce::JSON::parse(response);

        // Callback on message thread
        juce::MessageManager::callAsync([=]() { callback(json); });
    });
}
```

### Python HTTP Server

```python
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/process', methods=['POST'])
def process():
    data = request.json

    # 1. Analyze audio if provided
    analysis = None
    if data.get('audio_path'):
        analysis = analyze_audio(data['audio_path'])

    # 2. Build context and call LLM
    response = orchestrator.process(
        user_message=data['user_message'],
        audio_analysis=analysis,
        session_context=data.get('context', {})
    )

    return jsonify(response)

@app.route('/health', methods=['GET'])
def health():
    return jsonify({"status": "ok", "version": "0.1.0"})

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=8420)
```

---

## Audio Analysis Integration

### Cloud Option: Klang.io (or similar)

```python
import requests

def analyze_with_klang(audio_path: str) -> dict:
    with open(audio_path, 'rb') as f:
        response = requests.post(
            'https://api.klang.io/analyze',
            headers={'Authorization': f'Bearer {API_KEY}'},
            files={'audio': f}
        )

    data = response.json()
    return {
        'bpm': data['tempo'],
        'key': data['key'],
        'chords': data['chords'],
        'sections': data['sections'],
        'mood': data.get('mood', '')
    }
```

### Offline Option: Essentia (bundled)

[Essentia](https://essentia.upf.edu/) is an open-source audio analysis library.

```python
import essentia.standard as es

def analyze_with_essentia(audio_path: str) -> dict:
    # Load audio
    loader = es.MonoLoader(filename=audio_path)
    audio = loader()

    # Extract features
    rhythm_extractor = es.RhythmExtractor2013()
    bpm, beats, _, _, _ = rhythm_extractor(audio)

    key_extractor = es.KeyExtractor()
    key, scale, strength = key_extractor(audio)

    return {
        'bpm': round(bpm),
        'key': f"{key} {scale}",
        'chords': [],  # Chord detection is more complex
        'sections': [],
        'mood': ''
    }
```

---

## Component Breakdown (C++ Side)

### 1. Audio Engine

The heart of the application. Runs on a dedicated high-priority thread.

```cpp
class AudioEngine : public juce::AudioSource {
    // Audio callback - runs every buffer (e.g., every 5ms at 512 samples/48kHz)
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Mixer bus
    std::vector<std::unique_ptr<Track>> tracks;

    // Master output
    MasterBus masterBus;

    // Transport
    TransportState transport;
};
```

**Key responsibilities:**
- Mix all tracks to stereo output
- Handle recording from input
- Manage transport (play, stop, seek)
- Sync plugins to tempo/position

### 2. Track

A single channel strip with audio/MIDI routing.

```cpp
class Track {
    juce::String name;

    // Audio chain
    std::vector<std::unique_ptr<PluginSlot>> plugins;

    // Clips in arrangement
    std::vector<std::unique_ptr<Clip>> clips;

    // Mixer controls
    float volume;      // 0.0 - 1.0
    float pan;         // -1.0 (L) to 1.0 (R)
    bool mute;
    bool solo;

    // Routing
    AudioSource* input;   // Recording source
    AudioBus* output;     // Where to send output
};
```

### 3. Clip

A region of audio or MIDI data on the timeline.

```cpp
class Clip {
    double startTime;     // In beats or seconds
    double duration;
    double offset;        // Start offset within source

    // Either audio or MIDI, not both
    std::unique_ptr<AudioBuffer> audioData;
    std::unique_ptr<MidiSequence> midiData;

    // Visual
    juce::Colour color;
    juce::String name;
};
```

### 4. Plugin Host

Loads and manages VST3 (and AU on macOS) plugins.

```cpp
class PluginHost {
    juce::AudioPluginFormatManager formatManager;
    juce::KnownPluginList knownPlugins;

    // Scan for plugins
    void scanForPlugins();

    // Load a plugin instance
    std::unique_ptr<juce::AudioPluginInstance> loadPlugin(const juce::PluginDescription& desc);
};
```

### 5. Session State

Central state management for the entire project.

```cpp
class Session {
    // Metadata
    juce::String name;
    double tempo;
    int timeSignatureNumerator;
    int timeSignatureDenominator;

    // Content
    std::vector<std::unique_ptr<Track>> tracks;
    std::vector<Marker> markers;

    // Transport
    double playheadPosition;
    bool isPlaying;
    bool isRecording;

    // Undo
    juce::UndoManager undoManager;

    // Serialize/deserialize
    juce::ValueTree toValueTree();
    static Session fromValueTree(const juce::ValueTree& tree);
};
```

### 6. AI Bridge

C++ interface to the Python service.

```cpp
class AIBridge {
    std::unique_ptr<juce::ChildProcess> pythonProcess;

    // Start Python service on app launch
    void startService();

    // Send request to Python service
    void sendRequest(const juce::var& request,
                     std::function<void(juce::var)> onResponse);

    // Export audio for analysis
    juce::File exportAudioRegion(Track* track, double start, double end);

    // Execute commands from AI response
    void executeCommands(Session& session, const juce::Array<juce::var>& commands);
};
```

---

## Threading Model

```
┌─────────────────────────────────────────────────────────────────┐
│                      Main Thread (UI)                            │
│  - All UI rendering and interaction                              │
│  - Session state modifications (via message queue)               │
│  - AI response handling                                          │
└─────────────────────────────────────────────────────────────────┘
                               │
                        Message Queue
                               │
┌─────────────────────────────────────────────────────────────────┐
│                   Audio Thread (Real-time)                       │
│  - Audio callback                                                │
│  - Plugin processing                                             │
│  - NO allocations, NO locks, NO file I/O, NO HTTP               │
└─────────────────────────────────────────────────────────────────┘
                               │
┌─────────────────────────────────────────────────────────────────┐
│                 Background Threads (Workers)                     │
│  - File I/O                                                     │
│  - HTTP calls to Python service                                  │
│  - Plugin scanning                                               │
│  - Waveform rendering                                            │
└─────────────────────────────────────────────────────────────────┘
                               │
┌─────────────────────────────────────────────────────────────────┐
│              Python Process (Separate Process)                   │
│  - AI orchestration                                              │
│  - LLM API calls                                                 │
│  - Audio analysis                                                │
│  - Runs independently, communicates via HTTP                     │
└─────────────────────────────────────────────────────────────────┘
```

**Critical rule**: The audio thread must never block. All communication uses lock-free queues (audio) or async HTTP (AI).

---

## File Format

### Project File: `.daiw`

A directory bundle (like `.band` for GarageBand):

```
MyProject.daiw/
├── project.json          # Session metadata, track structure
├── audio/
│   ├── recording_001.wav
│   ├── recording_002.wav
│   └── ...
├── midi/
│   ├── clip_001.mid
│   └── ...
├── plugins/
│   └── states/           # Plugin preset states
│       ├── track1_eq.fxp
│       └── ...
└── ai/
    └── history.json      # Conversation history
```

### Interchange: Standard formats

- Audio: WAV, AIFF, FLAC, MP3 (import)
- MIDI: Standard MIDI File
- Export: WAV, MP3, FLAC

---

## Configuration

User settings stored in:
```
~/Library/Application Support/DAIW/
├── config.json           # API keys, preferences, offline mode
├── plugins/
│   └── cache.json        # Scanned plugin list
├── prompts/              # Custom/updated prompts (overrides bundled)
└── sessions/             # Recent sessions
```

### config.json

```json
{
  "version": 1,
  "ai": {
    "provider": "anthropic",
    "model": "claude-sonnet-4-20250514",
    "api_key": "sk-ant-...",
    "offline_mode": false
  },
  "analysis": {
    "provider": "klang",
    "api_key": "...",
    "fallback_to_local": true
  },
  "audio": {
    "sample_rate": 48000,
    "buffer_size": 512,
    "input_device": "SSL 2",
    "output_device": "SSL 2"
  }
}
```

---

## Plugin Hosting

### VST3 Support

JUCE provides `VST3PluginFormat` for loading VST3 plugins:

```cpp
// Scan for VST3 plugins
juce::VST3PluginFormat vst3Format;
juce::PluginDirectoryScanner scanner(
    knownPlugins,
    vst3Format,
    juce::FileSearchPath("/Library/Audio/Plug-Ins/VST3"),
    true,  // recursive
    deadMansPedalFile
);
```

### Built-in Plugins

For MVP, implement simple versions:
- **EQ**: 3-band parametric (low shelf, mid peak, high shelf)
- **Compressor**: Basic dynamics (threshold, ratio, attack, release)
- **Reverb**: Algorithmic reverb (room size, damping, wet/dry)

These can be JUCE `AudioProcessor` subclasses, same interface as external plugins.

---

## Security Considerations

- **API keys**: Stored locally in user's config, never sent to our servers
- **Audio files**: Processed locally or sent directly to user's chosen API
- **Python service**: Runs on localhost only, not accessible externally
- **No telemetry**: We don't collect usage data (user can opt-in later)
