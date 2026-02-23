# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

DAIW (Digital AI Workstation) is an AI-native DAW being built from scratch. The project is currently in the planning/documentation phase with no code written yet.

**Vision**: A DAW where AI is foundational (conversation-first interface) rather than bolted on as a plugin. Think Cursor for music production.

## Architecture

Two-process architecture:
- **C++/JUCE**: Audio engine, plugin hosting (VST3/AU), UI rendering
- **Python**: AI orchestration, LLM integration (Claude/OpenAI), audio analysis

Communication: Local HTTP on `localhost:8420` (C++ → Python)

## Build Commands (Once Code Exists)

```bash
# C++ (JUCE app)
mkdir build && cd build
cmake .. -G Xcode        # Generate Xcode project
cmake --build . --config Debug
./DAIW_artefacts/Debug/DAIW.app/Contents/MacOS/DAIW

# Python AI service
cd ai-service
python3.11 -m venv venv && source venv/bin/activate
pip install -r requirements.txt
uvicorn service:app --reload --port 8420
```

## Planned Project Structure

```
DAIW/
├── CMakeLists.txt
├── JUCE/                    # Submodule
├── src/
│   ├── Audio/               # AudioEngine, Track, Clip
│   ├── Session/             # Session state, Transport
│   ├── Plugins/             # VST3/AU hosting
│   ├── UI/                  # ArrangementView, MixerView, ChatPanel
│   └── AI/                  # AIBridge (HTTP client)
├── ai-service/
│   ├── service.py           # FastAPI server
│   ├── orchestrator.py      # AI logic
│   └── providers/           # anthropic.py, openai.py
└── docs/                    # Current planning docs
```

## Key Constraints

- **macOS only** for MVP (CoreAudio)
- **Audio thread**: No allocations, locks, file I/O, or HTTP calls
- **JUCE patterns**: Use ValueTree for serialization, juce::String, JUCE smart pointers
- **Original code**: Study open source DAWs for patterns but write original implementations

## Development Philosophy

1. Ship early, iterate fast
2. Audio first, UI second
3. Defer aggressively - add complexity only when simple fails
4. Manual testing first, automated tests for critical paths when stable
