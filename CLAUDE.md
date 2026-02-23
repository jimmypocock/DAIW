# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

DAIW (Digital AI Workstation) is an AI-native DAW being built from scratch.

**Vision**: A DAW where AI is foundational (conversation-first interface) rather than bolted on as a plugin. Think Cursor for music production.

**Status**: Active development - Phase 1 complete (audio passthrough).

## Build Commands

```bash
# Build and run (recommended)
make run

# Or manually:
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/DAIW_artefacts/Debug/DAIW.app/Contents/MacOS/DAIW

# Other useful commands
make build      # Build only
make clean      # Clean build directory
make rebuild    # Clean + build
make xcode      # Generate Xcode project for debugging
```

## Project Structure

```
DAIW/
├── CMakeLists.txt
├── Makefile                 # Build shortcuts
├── JUCE/                    # Submodule (v8.0.12)
├── src/
│   ├── Main.cpp             # App entry point, menu bar
│   ├── MainComponent.*      # Main window, level meters
│   ├── Audio/
│   │   └── AudioEngine.*    # Device management, passthrough, levels
│   └── UI/
│       ├── LookAndFeel/
│       │   └── DAIWLookAndFeel.*  # Synthwave color palette, theming
│       ├── Components/
│       │   └── LevelMeter.*       # Audio level visualization
│       ├── SettingsWindow.*       # Modal settings overlay
│       └── AudioSettingsPanel.*   # Device selection UI
├── ai-service/              # Python AI service (not yet integrated)
│   ├── service.py
│   └── requirements.txt
└── docs/                    # Planning docs
```

## Architecture

Two-process architecture:
- **C++/JUCE**: Audio engine, plugin hosting (VST3/AU), UI rendering
- **Python**: AI orchestration, LLM integration (Claude/OpenAI), audio analysis

Communication: Local HTTP on `localhost:8420` (C++ → Python)

## Key Patterns

- **Audio thread safety**: Use `std::atomic` for communication between audio and UI threads. No allocations, locks, file I/O, or HTTP calls on audio thread.
- **LookAndFeel**: All styling goes through `DAIWLookAndFeel` for consistency
- **Settings**: Use `SettingsWindow` modal pattern for configuration UIs
- **Levels**: `AudioEngine` exposes thread-safe level getters; UI polls via timer

## Key Constraints

- **macOS only** for MVP (CoreAudio)
- **JUCE patterns**: Use ValueTree for serialization, juce::String, JUCE smart pointers
- **Original code**: Study open source DAWs for patterns but write original implementations

## Development Philosophy

1. Ship early, iterate fast
2. Audio first, UI second
3. Defer aggressively - add complexity only when simple fails
4. Manual testing first, automated tests for critical paths when stable

## Development Roadmap

See `docs/ROADMAP.md` for current progress and next phases.
