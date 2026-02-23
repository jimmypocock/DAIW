# DAIW MVP Scope (v0.1)

## Goal

Build a functional DAW that proves the AI-native concept. Someone should be able to record and produce a real song using DAIW.

**Target user for MVP**: A musician with an audio interface (SSL 2 or similar) who wants to make music with AI assistance.

## What's IN for v0.1

### Audio Engine

- [ ] **Audio I/O via CoreAudio**
  - Select input/output device
  - Configure sample rate (44.1k, 48k, 96k)
  - Configure buffer size (128, 256, 512, 1024)
  - USB audio interface support (SSL 2 as reference)

- [ ] **Multi-track playback**
  - Play audio clips on timeline
  - Mix multiple tracks to stereo output
  - Basic summing and gain staging

- [ ] **Recording**
  - Record audio from input to track
  - Punch in/out
  - Monitor input while recording

- [ ] **Transport**
  - Play / Stop / Record
  - Playhead position (click to seek)
  - Tempo setting (BPM)
  - Time signature

### Tracks & Mixer

- [ ] **Track types**
  - Audio tracks (record/playback audio)
  - MIDI tracks (for virtual instruments) - stretch goal

- [ ] **Mixer controls per track**
  - Volume fader (with meter)
  - Pan knob
  - Mute / Solo buttons
  - Track name

- [ ] **Master bus**
  - Master volume
  - Master meter

### Arrangement View

- [ ] **Timeline**
  - Horizontal scrolling
  - Zoom in/out
  - Beat/bar grid
  - Playhead indicator

- [ ] **Clips**
  - Display audio waveforms
  - Move clips (drag)
  - Resize clips (trim)
  - Delete clips

- [ ] **Markers**
  - Add markers at positions
  - Name markers (Intro, Verse, Chorus, etc.)
  - Jump to marker

- [ ] **Track lanes**
  - Vertical list of tracks
  - Reorder tracks (drag)
  - Add/remove tracks

### Plugins

- [ ] **VST3 hosting**
  - Scan for installed VST3 plugins
  - Load plugin on track insert slot
  - Open plugin UI window
  - Automate plugin parameters (stretch)

- [ ] **Built-in effects**
  - EQ (3-band parametric)
  - Compressor (basic dynamics)
  - Reverb (algorithmic)

- [ ] **Plugin chain**
  - Multiple plugins per track
  - Reorder plugins
  - Bypass individual plugins

### AI Service (Python)

- [ ] **Bundled Python service**
  - Python 3.11 runtime bundled in .app
  - Local HTTP server on localhost:8420
  - Auto-start on app launch
  - Health check endpoint (`/health`)

- [ ] **AI orchestration**
  - LLM provider abstraction (Anthropic, OpenAI)
  - Prompt management (system prompts, context injection)
  - Command parsing and validation
  - Response formatting

- [ ] **Audio analysis integration**
  - Send audio clips to analysis API (Klang.io)
  - Extract: BPM, key, chords, energy, sections
  - Structure analysis results for LLM context
  - Cache analysis results per clip

- [ ] **MIDI generation**
  - LLM generates MIDI data as JSON
  - Validate MIDI output (ranges, timing)
  - Return structured MIDI to C++ engine

### AI Integration (C++ ↔ Python)

- [ ] **Chat panel UI**
  - Persistent panel at bottom
  - Message input with keyboard shortcuts
  - Message history (scrollable)
  - Clear distinction: user / AI / system messages
  - "Thinking" indicator during AI processing

- [ ] **IPC layer (C++ side)**
  - HTTP client for Python service communication
  - Non-blocking async requests
  - Timeout handling and error recovery
  - Service health monitoring

- [ ] **AI commands** (port from ChatM4L)
  - `set_volume` - Set track volume
  - `set_pan` - Set track pan
  - `set_mute` - Mute/unmute track
  - `set_solo` - Solo/unsolo track
  - `set_parameter` - Set plugin parameter
  - `add_notes` - Add MIDI notes to clip
  - `create_clip` - Create empty clip
  - `delete_clip` - Remove clip
  - `analyze_audio` - Trigger audio analysis

- [ ] **Context building**
  - Gather session state (tracks, clips, plugins)
  - Include audio analysis results
  - Send to Python service as JSON
  - Handle large context efficiently

- [ ] **Provider support**
  - Anthropic (Claude)
  - OpenAI (GPT-4)
  - API key configuration (stored securely)
  - Model selection per provider

### File Operations

- [ ] **Save/Load projects**
  - `.daiw` project format
  - Auto-save

- [ ] **Import audio**
  - WAV, AIFF, MP3, FLAC
  - Drag & drop onto tracks

- [ ] **Export**
  - Bounce to WAV
  - Select range (full song or selection)

### UI

- [ ] **Main window layout**
  - Arrangement view (primary)
  - Mixer view (toggleable or split)
  - Chat panel (persistent sidebar)

- [ ] **Keyboard shortcuts**
  - Space: Play/Stop
  - R: Record
  - Cmd+S: Save
  - Cmd+Z: Undo
  - Delete: Delete selected

- [ ] **Basic theming**
  - Dark theme (standard for DAWs)
  - Readable text, clear visual hierarchy

---

## What's OUT for v0.1

### Deferred to v0.2+

| Feature | Why deferred |
|---------|--------------|
| **Clip view / Session view** | Arrangement-first for MVP |
| **MIDI editing** | Complex, audio-first approach |
| **Automation lanes** | Nice to have, not essential |
| **Time stretching** | Complex DSP |
| **Pitch shifting** | Complex DSP |
| **Sidechain routing** | Advanced mixing feature |
| **Aux/Send tracks** | Can use plugins directly for now |
| **Advanced metering** | Basic meters sufficient |
| **Plugin delay compensation** | Important but complex |
| **Multi-output plugins** | Edge case |
| **Surround sound** | Stereo first |
| **Video sync** | Future feature |

### Explicitly Not Building

| Feature | Reason |
|---------|--------|
| **Windows/Linux support** | macOS first, port later |
| **Mobile apps** | Desktop focus |
| **Cloud sync** | Local-first |
| **Collaboration** | Single-user for now |
| **Marketplace** | Future business model |
| **Built-in instruments** | Use VST3 for now |

---

## Implementation Phases

### Phase 1: Foundation (Weeks 1-2)
- [ ] JUCE project setup
- [ ] Basic window with placeholder UI
- [ ] Audio device selection
- [ ] Audio passthrough (input → output)
- [ ] Play a single audio file

### Phase 2: Session Structure (Weeks 3-4)
- [ ] Track model
- [ ] Clip model
- [ ] Timeline rendering (basic)
- [ ] Multi-track playback
- [ ] Save/load project

### Phase 3: Recording (Weeks 5-6)
- [ ] Record audio to track
- [ ] Waveform display
- [ ] Transport controls
- [ ] Markers

### Phase 4: Mixing (Weeks 7-8)
- [ ] Mixer view
- [ ] Volume/pan/mute/solo
- [ ] Plugin hosting (VST3)
- [ ] Built-in effects (EQ, Comp, Reverb)

### Phase 5: AI Service (Weeks 9-10)
- [ ] Python service skeleton (FastAPI)
- [ ] Service bundling in .app
- [ ] Auto-start and health monitoring
- [ ] LLM provider integration (Anthropic/OpenAI)
- [ ] Audio analysis API integration (Klang.io)
- [ ] Prompt templates and context formatting

### Phase 6: AI Integration (Weeks 11-12)
- [ ] Chat panel UI (C++)
- [ ] C++ HTTP client for IPC
- [ ] Command execution pipeline
- [ ] Context building from session state
- [ ] MIDI generation and injection

### Phase 7: Polish (Weeks 13-14)
- [ ] Export functionality
- [ ] Keyboard shortcuts
- [ ] UI polish
- [ ] Bug fixes
- [ ] Documentation
- [ ] App bundling and distribution

---

## Success Criteria

v0.1 is complete when:

1. **Recording**: Can record vocals/guitar through SSL 2
2. **Arranging**: Can place and move clips on timeline
3. **Mixing**: Can adjust levels, pan, add plugins
4. **Effects**: Can load VST3 plugins and built-in effects
5. **AI Chat**: Can control session via natural language chat
6. **AI Analysis**: AI can analyze a recording and understand key/tempo
7. **AI Generation**: AI can generate MIDI drums/bass to accompany recording
8. **Export**: Can export a stereo WAV file
9. **Single Bundle**: Ships as one .app with everything bundled
10. **Real song**: Someone actually makes a song with it

---

## Risks & Mitigations

| Risk | Mitigation |
|------|------------|
| **JUCE learning curve** | Start with tutorials, reference existing projects |
| **Audio threading complexity** | Follow JUCE best practices, don't reinvent |
| **VST3 hosting edge cases** | Test with common plugins first, handle failures gracefully |
| **Scope creep** | Strict MVP boundaries, defer aggressively |
| **AI latency** | Non-blocking API calls, show "thinking" indicator |
| **Python bundling** | Use PyInstaller/py2app, test on clean macOS install |
| **IPC reliability** | Robust health checks, auto-restart on failure |
| **Audio analysis costs** | Start with Klang.io free tier, cache results aggressively |
| **Service startup time** | Pre-warm service on app launch, lazy loading |
| **App code signing** | Plan for notarization early, test bundled Python |

---

## Reference DAWs

Study these for UX patterns:

- **Ableton Live**: Session/arrangement duality, clean UI
- **Logic Pro**: Native Mac feel, comprehensive
- **Reaper**: Efficient, customizable, small team built it
- **GarageBand**: Simple, approachable, good onboarding

Reaper is especially relevant - proof that a small team can build a professional DAW.
