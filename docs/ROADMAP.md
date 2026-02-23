# DAIW Development Roadmap

Progress tracker for the DAIW build phases.

## Phase Status

| Phase | Description | Status | Notes |
|-------|-------------|--------|-------|
| **1** | Audio passthrough (input → output) | ✅ Complete | Device selection, settings UI, level meters |
| **2** | Play a WAV file | 🔲 Not started | Audio formats, file I/O, transport controls |
| **3** | Multiple tracks mixing | 🔲 Not started | Buffer management, gain staging, summing |
| **4** | Basic recording | 🔲 Not started | Input monitoring, file writing |
| **5** | Timeline UI with waveforms | 🔲 Not started | Custom components, graphics, scrolling |
| **6** | Load and host a VST3 plugin | 🔲 Not started | Plugin hosting, parameter control |
| **7** | MIDI playback | 🔲 Not started | MIDI sequencing, timing |
| **8** | Python service integration | 🔲 Not started | IPC, HTTP client, subprocess management |

## Phase 1 Details (Complete)

**What was built:**
- `AudioEngine` class with CoreAudio device management
- Input → output passthrough with configurable latency
- Settings modal with sidebar navigation (⌘,)
- Audio device selection (input/output/sample rate/buffer size)
- Stereo level meters with RMS calculation and smooth decay
- `DAIWLookAndFeel` with synthwave color palette
- Menu bar integration

**Key files:**
- `src/Audio/AudioEngine.*`
- `src/UI/SettingsWindow.*`
- `src/UI/AudioSettingsPanel.*`
- `src/UI/Components/LevelMeter.*`
- `src/UI/LookAndFeel/DAIWLookAndFeel.*`

## Phase 2 Preview

**Goal:** Load and play a WAV file with transport controls (play/pause/stop).

**What to build:**
- `AudioFilePlayer` or extend `AudioEngine` to load audio files
- Transport controls (play, pause, stop, seek)
- Basic timeline/playhead position display
- File browser or drag-and-drop to load files

**JUCE classes to use:**
- `juce::AudioFormatManager` - register audio formats
- `juce::AudioFormatReader` - read audio files
- `juce::AudioTransportSource` - transport control
- `juce::FileChooser` - file selection dialog

## Future Phases (High-Level)

**Phase 3-4 (Multi-track + Recording):**
- Track class with clips, volume, pan, mute/solo
- Mixer view with channel strips
- Recording to disk with monitoring

**Phase 5-6 (Timeline + Plugins):**
- Arrangement view with draggable clips
- Waveform rendering
- VST3/AU plugin hosting and scanning
- Plugin UI windows

**Phase 7-8 (MIDI + AI):**
- MIDI track support
- Piano roll editor
- Python AI service integration
- Chat panel for AI interaction
