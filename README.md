# DAIW

**Digital AI Workstation** — An AI-native DAW where AI is foundational, not a plugin.

Think Cursor for music production: conversation-first interface, AI that understands your session, and seamless integration with modern LLMs.

## Status

🚧 **Early Development** — Building the core audio engine and architecture.

## Architecture

Two-process design:
- **C++/JUCE** — Audio engine, VST3/AU plugin hosting, UI
- **Python** — AI orchestration, LLM integration (Claude/OpenAI), audio analysis

Communication via local HTTP on `localhost:8420`.

## Building

**Prerequisites:** macOS, Xcode command line tools, CMake, Ninja, Python 3.11+

```bash
# Clone with submodules
git clone --recursive https://github.com/jimmypocock/DAIW.git
cd DAIW

# Build and run
make run
```

**Other commands:**
```bash
make build      # Build only
make clean      # Clean build directory
make xcode      # Generate Xcode project for debugging
```

## Documentation

See [`docs/`](docs/) for detailed planning and architecture:
- [Vision](docs/VISION.md) — Why we're building this
- [Architecture](docs/ARCHITECTURE.md) — Technical design
- [MVP](docs/MVP.md) — Minimum viable product scope
- [Getting Started](docs/GETTING_STARTED.md) — Development setup

## License

[MIT](LICENSE)
