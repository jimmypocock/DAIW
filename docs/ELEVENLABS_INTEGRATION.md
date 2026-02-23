# ElevenLabs Music Integration

> **Status**: Ready for Implementation (API available since August 2025)
> **Last Updated**: February 2026

## Overview

ElevenLabs Music is an AI audio generation service that creates studio-quality music from natural language prompts. This document outlines how it integrates into DAIW as an optional audio generation provider.

**API Availability:** The Eleven Music API launched August 18, 2025 and is available to paid ElevenLabs users. It's the first music API trained on licensed data and cleared for broad commercial use.

## Vision

### The Gap in Current Architecture

DAIW's MVP focuses on **MIDI generation** - the AI understands musical intent and outputs MIDI data that plays through the user's VST instruments. This works well when users have the right plugins, but has limitations:

| Scenario | MIDI Generation | Audio Generation |
|----------|-----------------|------------------|
| User has good drum VST | ✅ Perfect | Unnecessary |
| User wants specific synth sound | ✅ Use their plugin | Could work |
| User lacks instruments | ❌ Nothing to play MIDI | ✅ Generates the audio |
| Quick idea exploration | Requires setup | ✅ Instant results |
| Vocals/harmonies | ❌ Not possible | ✅ Core strength |
| Scratch tracks for songwriting | ❌ Limited | ✅ Ideal use case |

### ElevenLabs Fills the Audio Gap

With ElevenLabs integration, DAIW becomes a complete creative partner:

```
"Add a funky bass line"
    │
    ├─► User has bass VST? → Generate MIDI → Play through their plugin
    │
    └─► User lacks bass VST? → Generate audio via ElevenLabs → Import as clip
```

### Use Cases

**1. Instrument Coverage**
- User recording guitar wants drums → Generate drum audio
- Bedroom producer lacks orchestral libraries → Generate string sections
- Singer-songwriter needs accompaniment → Generate piano/guitar backing

**2. Vocal Generation**
- Scratch vocals for songwriting ("sing this melody with 'la la la'")
- Harmony generation ("add a high harmony to this chorus")
- Demo vocals before hiring a singer
- Background vocals and ad-libs

**3. Creative Exploration**
- "What would this sound like with a reggae feel?"
- "Generate 3 different drum patterns I can choose from"
- "Make a lo-fi version of this section"
- Quick A/B comparison of styles

**4. Sound Design & Textures**
- Ambient pads and atmospheres
- Risers, impacts, transitions
- Textural elements that don't need MIDI precision

---

## ElevenLabs Capabilities

### API Details

**Endpoints:**
- `POST https://api.elevenlabs.io/v1/music` - Compose (returns complete file)
- `POST https://api.elevenlabs.io/v1/music/stream` - Stream (returns audio as it generates)

**Authentication:** `xi-api-key` header

### Technical Specs

| Parameter | Value |
|-----------|-------|
| Duration | 3 seconds - 10 minutes (3,000 - 600,000 ms) |
| Output Formats | MP3, PCM, Opus (various bitrates/sample rates) |
| MP3 192kbps | Requires Creator tier+ |
| PCM 44.1kHz | Requires Pro tier+ |
| Languages | English, Spanish, German, Japanese, and more |
| Content Types | Vocals, instrumentals, or both |

### Request Parameters

**Simple prompt mode:**
```json
{
  "prompt": "upbeat indie rock with driving drums",
  "music_length_ms": 30000,
  "force_instrumental": true,
  "model_id": "music_v1",
  "output_format": "mp3_44100_128"
}
```

**Composition plan mode (structured sections):**
```json
{
  "composition_plan": {
    "positive_global_styles": ["folk", "acoustic", "warm"],
    "negative_global_styles": ["electronic", "harsh"],
    "sections": [
      {
        "name": "verse",
        "duration_ms": 30000,
        "styles": ["mellow", "fingerpicked guitar"],
        "lyrics": "Walking down the road..."
      },
      {
        "name": "chorus",
        "duration_ms": 20000,
        "styles": ["building", "full band"],
        "lyrics": "This is where we belong..."
      }
    ]
  },
  "respect_sections_durations": true,
  "model_id": "music_v1"
}
```

### Features

- **Natural Language Prompts**: Describe what you want in plain English
- **Musical Terminology**: Understands genre, style, tempo, key, mood
- **Composition Plans**: Define sections with specific styles, durations, lyrics
- **Style Control**: Positive and negative style guidance
- **C2PA Signing**: Content authenticity signing (added January 2026)
- **Reproducible Seeds**: Optional seed parameter for similar outputs

### Pricing & Licensing

| Aspect | Details |
|--------|---------|
| Availability | Paid tiers only |
| Billing | Per generation |
| Standard use | Included (podcasts, social media, personal projects) |
| Commercial use | Additional license for ads, film, TV, games, enterprise |

### Streaming Support

The `/v1/music/stream` endpoint returns audio as `application/octet-stream`, enabling:
- Progressive audio loading during generation
- User can hear beginning of track while rest generates
- Better perceived latency for longer generations

```python
# Streaming example
response = requests.post(
    "https://api.elevenlabs.io/v1/music/stream",
    headers={"xi-api-key": api_key},
    json={"prompt": "chill lo-fi beat", "music_length_ms": 60000},
    stream=True
)

with open("output.mp3", "wb") as f:
    for chunk in response.iter_content(chunk_size=8192):
        f.write(chunk)
        # Could also pipe to audio player for live preview
```

### Limitations

- Not real-time (generation takes time, even with streaming)
- Requires internet connection
- Per-generation costs (can add up quickly)
- May not perfectly match existing session context
- Exact reproducibility not guaranteed even with seed
- Streaming requires careful buffer management for preview playback

---

## Architecture

### Integration Point

ElevenLabs fits into the existing two-process architecture as an additional provider in the Python AI service:

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
│   │         ▲                                      │                   │  │
│   │         │ Import generated audio               │                   │  │
│   │         │                                      │                   │  │
│   └─────────┼──────────────────────────────────────┼───────────────────┘  │
│             │                                      │                       │
│             │                          HTTP (localhost:8420)               │
│             │                                      │                       │
│   ┌─────────┼──────────────────────────────────────┼───────────────────┐  │
│   │         │          Python (AI Service)         │                   │  │
│   │         │                                      ▼                   │  │
│   │   ┌─────┴───────┐  ┌─────────────┐  ┌─────────────────────────┐   │  │
│   │   │   Audio     │  │     AI      │  │     Provider Clients    │   │  │
│   │   │  Importer   │  │ Orchestrator│  │                         │   │  │
│   │   │             │  │             │  │  ┌─────────────────┐    │   │  │
│   │   └─────────────┘  └──────┬──────┘  │  │ Claude/OpenAI   │    │   │  │
│   │                           │         │  │ (Text/MIDI)     │    │   │  │
│   │                           │         │  └─────────────────┘    │   │  │
│   │                           │         │  ┌─────────────────┐    │   │  │
│   │                           │         │  │ ElevenLabs      │    │   │  │
│   │                           ├────────►│  │ (Audio Gen)     │    │   │  │
│   │                           │         │  └─────────────────┘    │   │  │
│   │                           │         │  ┌─────────────────┐    │   │  │
│   │                           │         │  │ Klang.io        │    │   │  │
│   │                           │         │  │ (Analysis)      │    │   │  │
│   │                           │         │  └─────────────────┘    │   │  │
│   │                           │         └─────────────────────────┘   │  │
│   │                           │                                       │  │
│   └───────────────────────────┼───────────────────────────────────────┘  │
│                               │                                           │
└───────────────────────────────┼───────────────────────────────────────────┘
                                │
                                ▼
                    ┌───────────────────────┐
                    │   ElevenLabs API      │
                    │   api.elevenlabs.io   │
                    └───────────────────────┘
```

### Decision Flow

The AI Orchestrator decides whether to use MIDI or audio generation:

```
User Request
    │
    ▼
┌─────────────────────────────────────┐
│         AI Orchestrator             │
│                                     │
│  1. Analyze request intent          │
│  2. Check session context           │
│  3. Evaluate available resources    │
│                                     │
│  Decision factors:                  │
│  - Does user have relevant VST?     │
│  - Is this a vocal request?         │
│  - User preference in config?       │
│  - Is audio generation enabled?     │
│                                     │
└─────────────┬───────────────────────┘
              │
              ▼
    ┌─────────┴─────────┐
    │                   │
    ▼                   ▼
┌───────────┐     ┌───────────────┐
│   MIDI    │     │    Audio      │
│ Generation│     │  Generation   │
│           │     │               │
│ - LLM     │     │ - ElevenLabs  │
│ - JSON    │     │ - Download    │
│ - Notes   │     │ - Import      │
└───────────┘     └───────────────┘
```

### Request/Response Flow

```
1. User: "Add drums that match my recording"
                    │
                    ▼
2. C++ App: Gathers context, exports audio region
                    │
   POST /process    │
   {                │
     "user_message": "Add drums that match my recording",
     "audio_path": "/tmp/daiw_audio_12345.wav",
     "context": {
       "tracks": [...],
       "tempo": 95,
       "time_signature": "4/4",
       "position": 0.0,
       "available_instruments": ["piano", "guitar"]  // No drums VST
     },
     "preferences": {
       "audio_generation": "enabled",
       "prefer_midi": false
     }
   }                │
                    ▼
3. Python Service: Analyzes audio, determines approach
                    │
   - BPM: 95, Key: G major, Mood: folk
   - User lacks drum VST
   - Audio generation enabled
   - Decision: Use ElevenLabs
                    │
                    ▼
4. ElevenLabs API Call:
   {
     "prompt": "acoustic brush drums, folk style, 95 BPM,
               G major, mellow feel, 8 bars",
     "duration": 16,  // seconds (8 bars at 95 BPM)
     "instrumental": true
   }
                    │
                    ▼
5. Download generated audio to temp file
   /tmp/daiw_generated_drums_67890.mp3
                    │
                    ▼
6. Response to C++ App:
   {
     "response": "I've generated a folk-style brush drum pattern
                  that matches your recording's tempo and feel.",
     "commands": [{
       "action": "import_audio",
       "source_path": "/tmp/daiw_generated_drums_67890.mp3",
       "target_track": "Drums (Generated)",
       "position": 0.0,
       "create_track": true
     }]
   }
                    │
                    ▼
7. C++ App: Creates track, imports audio, updates UI
```

---

## Implementation

### Configuration

Add ElevenLabs to user config (`~/Library/Application Support/DAIW/config.json`):

```json
{
  "version": 1,
  "ai": {
    "provider": "anthropic",
    "model": "claude-sonnet-4-20250514",
    "api_key": "sk-ant-..."
  },
  "audio_generation": {
    "enabled": true,
    "provider": "elevenlabs",
    "api_key": "xi-...",
    "preferences": {
      "prefer_midi_when_available": true,
      "auto_create_tracks": true,
      "default_format": "wav"
    }
  },
  "analysis": {
    "provider": "klang",
    "api_key": "..."
  }
}
```

### Python Service: ElevenLabs Client

```python
# ai-service/providers/elevenlabs.py

import os
import requests
import tempfile
from typing import Optional, List
from dataclasses import dataclass

@dataclass
class GeneratedAudio:
    path: str
    duration_ms: int
    prompt: str

@dataclass
class CompositionSection:
    name: str
    duration_ms: int
    styles: List[str]
    lyrics: Optional[str] = None

class ElevenLabsClient:
    """Client for ElevenLabs Music API."""

    BASE_URL = "https://api.elevenlabs.io/v1"

    def __init__(self, api_key: str, output_format: str = "mp3_44100_128"):
        self.api_key = api_key
        self.output_format = output_format
        self.session = requests.Session()
        self.session.headers.update({
            "xi-api-key": api_key,
            "Content-Type": "application/json"
        })

    def generate_music(
        self,
        prompt: str,
        duration_ms: int = 30000,
        instrumental: bool = True,
        style_hints: Optional[dict] = None,
        seed: Optional[int] = None
    ) -> GeneratedAudio:
        """
        Generate music from a text prompt.

        Args:
            prompt: Natural language description of desired music
            duration_ms: Length in milliseconds (3000-600000)
            instrumental: If True, generate without vocals
            style_hints: Optional dict with bpm, key, genre, mood
            seed: Optional seed for reproducibility

        Returns:
            GeneratedAudio with path to downloaded file
        """
        # Build the full prompt with style hints
        full_prompt = self._build_prompt(prompt, style_hints)

        # Clamp duration to valid range
        duration_ms = max(3000, min(duration_ms, 600000))

        # Build request payload
        payload = {
            "prompt": full_prompt,
            "music_length_ms": duration_ms,
            "force_instrumental": instrumental,
            "model_id": "music_v1"
        }

        if seed is not None:
            payload["seed"] = seed

        # Make API request - returns binary audio directly
        response = self.session.post(
            f"{self.BASE_URL}/music",
            params={"output_format": self.output_format},
            json=payload,
            timeout=180  # Generation can take time for longer pieces
        )
        response.raise_for_status()

        # Save binary audio to temp file
        local_path = self._save_audio(response.content)

        return GeneratedAudio(
            path=local_path,
            duration_ms=duration_ms,
            prompt=full_prompt
        )

    def generate_with_composition_plan(
        self,
        sections: List[CompositionSection],
        positive_styles: List[str] = None,
        negative_styles: List[str] = None,
        respect_durations: bool = True
    ) -> GeneratedAudio:
        """
        Generate music from a structured composition plan.

        Args:
            sections: List of CompositionSection objects
            positive_styles: Global styles to encourage
            negative_styles: Global styles to avoid
            respect_durations: Whether to strictly enforce section timing

        Returns:
            GeneratedAudio with path to downloaded file
        """
        composition_plan = {
            "positive_global_styles": positive_styles or [],
            "negative_global_styles": negative_styles or [],
            "sections": [
                {
                    "name": s.name,
                    "duration_ms": s.duration_ms,
                    "styles": s.styles,
                    **({"lyrics": s.lyrics} if s.lyrics else {})
                }
                for s in sections
            ]
        }

        payload = {
            "composition_plan": composition_plan,
            "respect_sections_durations": respect_durations,
            "model_id": "music_v1"
        }

        response = self.session.post(
            f"{self.BASE_URL}/music",
            params={"output_format": self.output_format},
            json=payload,
            timeout=300  # Composition plans may take longer
        )
        response.raise_for_status()

        local_path = self._save_audio(response.content)
        total_duration = sum(s.duration_ms for s in sections)

        return GeneratedAudio(
            path=local_path,
            duration_ms=total_duration,
            prompt=f"Composition plan with {len(sections)} sections"
        )

    def _build_prompt(self, base_prompt: str, style_hints: Optional[dict]) -> str:
        """Enhance prompt with musical context from analysis."""
        if not style_hints:
            return base_prompt

        parts = [base_prompt]

        if "bpm" in style_hints and style_hints["bpm"]:
            parts.append(f"{style_hints['bpm']} BPM")
        if "key" in style_hints and style_hints["key"]:
            parts.append(f"in {style_hints['key']}")
        if "mood" in style_hints and style_hints["mood"]:
            parts.append(style_hints['mood'])
        if "genre" in style_hints and style_hints["genre"]:
            parts.append(f"{style_hints['genre']} style")

        return ", ".join(parts)

    def _save_audio(self, content: bytes) -> str:
        """Save audio bytes to temp file, return path."""
        # Determine extension from output format
        if self.output_format.startswith("mp3"):
            ext = ".mp3"
        elif self.output_format.startswith("pcm"):
            ext = ".wav"
        elif self.output_format.startswith("opus"):
            ext = ".opus"
        else:
            ext = ".mp3"

        fd, path = tempfile.mkstemp(suffix=ext, prefix="daiw_generated_")

        with os.fdopen(fd, 'wb') as f:
            f.write(content)

        return path

    def check_available(self) -> bool:
        """Check if API is accessible and key is valid."""
        try:
            # Use the user endpoint to verify API key
            response = self.session.get(
                f"{self.BASE_URL}/user",
                timeout=5
            )
            return response.status_code == 200
        except Exception:
            return False
```

### Python Service: Orchestrator Integration

```python
# ai-service/orchestrator.py

from providers.elevenlabs import ElevenLabsClient
from providers.anthropic import AnthropicClient
from analysis import analyze_audio

class AIOrchestrator:
    """Coordinates between LLM, audio analysis, and generation providers."""

    def __init__(self, config: dict):
        self.config = config
        self.llm = AnthropicClient(config["ai"]["api_key"])

        # Initialize ElevenLabs if configured
        self.audio_gen = None
        if config.get("audio_generation", {}).get("enabled"):
            api_key = config["audio_generation"].get("api_key")
            if api_key:
                self.audio_gen = ElevenLabsClient(api_key)

    def process(
        self,
        user_message: str,
        audio_path: str = None,
        session_context: dict = None
    ) -> dict:
        """
        Process a user request and return response with commands.
        """
        # 1. Analyze audio if provided
        analysis = None
        if audio_path:
            analysis = analyze_audio(audio_path)

        # 2. Determine if this needs audio generation
        needs_audio_gen = self._should_generate_audio(
            user_message,
            session_context,
            analysis
        )

        # 3. Get LLM response with appropriate instructions
        llm_response = self._get_llm_response(
            user_message,
            analysis,
            session_context,
            audio_generation_available=needs_audio_gen
        )

        # 4. If audio generation requested, do it
        commands = llm_response.get("commands", [])
        commands = self._process_audio_generation_commands(commands, analysis)

        return {
            "response": llm_response["response"],
            "commands": commands
        }

    def _should_generate_audio(
        self,
        message: str,
        context: dict,
        analysis: dict
    ) -> bool:
        """Determine if audio generation is appropriate for this request."""
        if not self.audio_gen:
            return False

        # Check user preferences
        if not self.config.get("audio_generation", {}).get("enabled"):
            return False

        # Vocal requests always use audio generation
        vocal_keywords = ["vocal", "sing", "voice", "harmony", "lyrics"]
        if any(kw in message.lower() for kw in vocal_keywords):
            return True

        # Check if user lacks the instrument
        requested_instrument = self._extract_instrument(message)
        if requested_instrument:
            available = context.get("available_instruments", [])
            if requested_instrument not in available:
                return True

        # Default: prefer MIDI if available
        return not self.config["audio_generation"].get("prefer_midi_when_available", True)

    def _process_audio_generation_commands(
        self,
        commands: list,
        analysis: dict
    ) -> list:
        """Process any audio generation commands, replacing with import commands."""
        processed = []

        for cmd in commands:
            if cmd.get("action") == "generate_audio":
                # Generate the audio
                style_hints = {
                    "bpm": analysis.get("bpm") if analysis else None,
                    "key": analysis.get("key") if analysis else None,
                    "mood": analysis.get("mood") if analysis else None
                }

                result = self.audio_gen.generate_music(
                    prompt=cmd["prompt"],
                    duration_seconds=cmd.get("duration", 16),
                    instrumental=cmd.get("instrumental", True),
                    style_hints=style_hints
                )

                # Replace with import command
                processed.append({
                    "action": "import_audio",
                    "source_path": result.path,
                    "target_track": cmd.get("target_track", "Generated Audio"),
                    "position": cmd.get("position", 0.0),
                    "create_track": cmd.get("create_track", True)
                })
            else:
                processed.append(cmd)

        return processed
```

### C++ Side: Command Handler

```cpp
// In CommandExecutor.cpp

void CommandExecutor::executeCommand(Session& session, const juce::var& command) {
    juce::String action = command["action"].toString();

    if (action == "import_audio") {
        executeImportAudio(session, command);
    }
    // ... other commands
}

void CommandExecutor::executeImportAudio(Session& session, const juce::var& command) {
    juce::String sourcePath = command["source_path"].toString();
    juce::String targetTrack = command["target_track"].toString();
    double position = command["position"];
    bool createTrack = command["create_track"];

    // Create track if needed
    Track* track = session.getTrackByName(targetTrack);
    if (!track && createTrack) {
        track = session.createTrack(targetTrack, TrackType::Audio);
    }

    if (!track) {
        // Handle error - track not found and couldn't create
        return;
    }

    // Import the audio file
    juce::File audioFile(sourcePath);
    if (!audioFile.existsAsFile()) {
        // Handle error - file not found
        return;
    }

    // Create audio clip from file
    auto clip = std::make_unique<AudioClip>(audioFile);
    clip->setPosition(position);

    // Add to track
    track->addClip(std::move(clip));

    // Clean up temp file
    audioFile.deleteFile();

    // Update UI
    sendChangeMessage();
}
```

### New API Endpoint

```python
# ai-service/service.py

@app.route('/generate', methods=['POST'])
def generate_audio():
    """
    Direct audio generation endpoint (for explicit /generate commands).

    Body: {
        "prompt": "funky bass line",
        "duration": 16,
        "instrumental": true,
        "context": { "bpm": 95, "key": "G major" }
    }
    """
    data = request.json

    if not orchestrator.audio_gen:
        return jsonify({
            "error": "Audio generation not configured",
            "hint": "Add ElevenLabs API key to config.json"
        }), 400

    try:
        result = orchestrator.audio_gen.generate_music(
            prompt=data["prompt"],
            duration_seconds=data.get("duration", 16),
            instrumental=data.get("instrumental", True),
            style_hints=data.get("context")
        )

        return jsonify({
            "audio_path": result.path,
            "duration": result.duration,
            "prompt_used": result.prompt
        })

    except Exception as e:
        return jsonify({"error": str(e)}), 500
```

### LLM System Prompt Addition

Add to system prompt when audio generation is available:

```markdown
## Audio Generation

You have access to AI audio generation via ElevenLabs. Use this when:
- The user requests vocals or harmonies
- The user lacks the necessary instrument plugin
- The user explicitly asks for generated audio

To generate audio, include a command like:
{
  "action": "generate_audio",
  "prompt": "description of the audio to generate",
  "duration": 16,  // seconds
  "instrumental": true,  // false for vocals
  "target_track": "Track Name",
  "position": 0.0,  // beats
  "create_track": true
}

The prompt should include:
- Instrument/sound type
- Style/genre
- Feel/mood
- Any specific characteristics

The system will automatically add tempo, key, and other context from the session.

Example prompts:
- "acoustic brush drums, laid back folk feel"
- "funky slap bass, tight and punchy"
- "ethereal pad, ambient and evolving"
- "female vocal harmony, ooh's and aah's, dreamy"
```

---

## User Experience

### UI Considerations

**1. Generation Indicator**
```
┌────────────────────────────────────────────────┐
│ 🎵 Generating drums...                         │
│ ████████████░░░░░░░░░░░░░░  45%               │
│                                                │
│ "acoustic brush drums, folk style, 95 BPM..."  │
└────────────────────────────────────────────────┘
```

**2. Generated Track Badge**
Show that a track contains AI-generated audio:
```
┌─────────────────────────────────────┐
│ 🤖 Drums (Generated)         [M][S] │
│ ▃▅▂▇▄▂▅▃▇▂▄▅▂▃▇▄▂▅▃▂▇▄▅▂▃  │
└─────────────────────────────────────┘
```

**3. Regenerate Option**
Right-click on generated clip:
- "Regenerate" - Same prompt, new generation
- "Edit prompt" - Modify and regenerate
- "Convert to regular clip" - Remove AI metadata

### Chat Interaction Examples

**Basic generation:**
```
User: Add some drums

AI: I notice you don't have a drum plugin loaded. Would you like me to
    generate a drum track that matches your recording?

    Based on my analysis:
    - Tempo: 95 BPM
    - Key: G major
    - Feel: Folk, acoustic, mellow

    [Generate Drums] [Add Drum Plugin Instead]
```

**With options:**
```
User: Generate a bass line

AI: I've generated a bass line that complements your track:

    🎵 Generated: "warm upright bass, folk style, 95 BPM, G major"

    [▶ Preview] [✓ Keep] [↻ Regenerate] [✎ Edit Prompt]
```

**Vocal generation:**
```
User: Add harmonies to the chorus

AI: I'll generate vocal harmonies for the chorus section (bars 17-24).

    What style of harmonies would you like?
    - [Tight, close harmonies]
    - [Wide, open harmonies]
    - [Ethereal, airy oohs]
    - [Let me describe...]
```

---

## Cost & Usage Considerations

### Pricing Model (Estimated)

ElevenLabs pricing TBD, but likely:
- Per-second or per-generation pricing
- Different tiers for quality/duration
- Possible subscription with credits

### Cost Management in DAIW

```json
// In config.json
{
  "audio_generation": {
    "enabled": true,
    "api_key": "xi-...",
    "budget": {
      "monthly_limit_usd": 20.00,
      "warn_at_usd": 15.00,
      "require_confirmation_above_usd": 1.00
    }
  }
}
```

```python
# In orchestrator
def _check_generation_budget(self, estimated_cost: float) -> bool:
    """Check if generation is within budget."""
    usage = self._get_monthly_usage()

    if usage + estimated_cost > self.config["budget"]["monthly_limit_usd"]:
        raise BudgetExceededError("Monthly audio generation limit reached")

    if usage + estimated_cost > self.config["budget"]["warn_at_usd"]:
        # Log warning, maybe show in UI
        pass

    return True
```

---

## Offline Behavior

Audio generation requires internet. When offline:

```python
def _should_generate_audio(self, ...):
    # Check connectivity
    if not self._is_online():
        return False

    if not self.audio_gen or not self.audio_gen.check_available():
        return False

    # ... rest of logic
```

The AI should gracefully fall back:

```
User: Add drums

AI: Audio generation is unavailable (offline). I can help you:

    1. Create a MIDI drum pattern (requires drum plugin)
    2. Suggest drum samples to import
    3. Wait until you're back online

    What would you prefer?
```

---

## Future Enhancements

### Phase 1: Basic Integration (Initial)
- Generate audio from prompts
- Import as clips
- Basic cost tracking

### Phase 2: Smart Integration
- Auto-detect when audio gen is better than MIDI
- Contextual prompt enhancement from session
- Preview before committing

### Phase 3: Advanced Features
- Section-by-section editing ("regenerate just the fill")
- Style transfer ("make this drum loop more jazzy")
- Stems generation (separate kick, snare, hi-hat)
- Audio-to-audio continuation

### Phase 4: Creative Tools
- Variation generation ("give me 5 options")
- A/B testing interface
- AI-suggested generations based on gaps in arrangement

---

## Dependencies

| Dependency | Purpose | Notes |
|------------|---------|-------|
| `requests` | HTTP client | Already in stack |
| `tempfile` | Temp file management | Python stdlib |
| ElevenLabs SDK | Official client | Use when available |

---

## Open Questions

1. ~~**API Availability**: When will ElevenLabs Music API be publicly available?~~ ✅ Available since August 2025
2. **Pricing Details**: Exact per-generation costs? Volume discounts?
3. **Audio Quality**: How does output quality compare for production use?
4. **Latency**: Typical generation time for various durations?
5. **Context Matching**: How well will it match existing session audio (BPM/key accuracy)?
6. **Stem Separation**: Will they offer stem output for easier mixing?
7. ~~**Streaming**: Is streaming output supported for progressive loading?~~ ✅ Yes, `/v1/music/stream` endpoint available
8. **Rate Limits**: What are the API rate limits per tier?

---

## References

- [Eleven Music Overview](https://elevenlabs.io/docs/overview/capabilities/music)
- [Compose Music API](https://elevenlabs.io/docs/api-reference/music/compose)
- [Stream Music API](https://elevenlabs.io/docs/api-reference/music/stream)
- [Eleven Music API Launch Announcement](https://elevenlabs.io/blog/eleven-music-now-available-in-the-api)
- [ElevenLabs Developer Portal](https://elevenlabs.io/developers)
- [DAIW Architecture](./ARCHITECTURE.md)
- [DAIW MVP Scope](./MVP.md)
