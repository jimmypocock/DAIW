# DAIW Design

## Design Philosophy

**Minimal UI, maximum capability.**

Traditional DAWs expose every feature as buttons, menus, and panels. DAIW exposes features through conversation. The UI shows what you need to *see*, the AI handles what you need to *do*.

### Guiding Principles

1. **Chat-first, not chat-only** - AI is primary interface, but visual feedback is essential
2. **Progressive disclosure** - Simple by default, detail on demand
3. **Respect muscle memory** - Space=play, familiar waveforms, time flows left-to-right
4. **Selection = context** - Click something, AI knows you're talking about it
5. **Undo everything** - Every AI action is undoable (Cmd+Z)
6. **MIDI is first-class** - Not an afterthought, core to music creation

---

## Layout

### Primary View: Arrangement + Chat

```
┌─────────────────────────────────────────────────────────────────────────┐
│  ▶  ⏹  ⏺  │  ♩ 120  │  4/4  │  1:32.4  │               [⚙]  [◱]      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│     1       5       9       13      17      21      25      29         │
│     ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───      │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │ Drums    [E][M][S]  ████████░░░░████████░░░░████████████████      │  │
│  │ Bass     [E][M][S]  ░░░░████████░░░░████████░░░░████████████      │  │
│  │ Vocals   [E][M][S]  ░░░░░░░░░░░░████████████████████░░░░░░░░      │  │
│  │ Synth    [E][M][S]  ████░░░░████░░░░████░░░░████░░░░████░░░░      │  │
│  │ Guitar   [E][M][S]  ░░░░░░░░████████████████░░░░░░░░████████      │  │
│  │                            ▼                                      │  │
│  │ + Add Track                                                       │  │
│  └──────────────────────────────────────────────────────────────────┘  │
│                                                                         │
├─────────────────────────────────────────────────────────────────────────┤
│ ┌─ Drums ──────────────────────────────────────────────────────────────┐│
│ │  Compressor → EQ → Reverb                    ═══════●══  -6dB ██▓░░ ││
│ └──────────────────────────────────────────────────────────────────────┘│
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  You: Add some verb to the snare, make it sound like a big room        │
│                                                                         │
│  AI: Added Reverb to Drums track. Set to large hall preset with        │
│  2.4s decay. [Open plugin]                                             │
│                                                                         │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │ >  Make it even bigger, like 80s gated reverb_                   │  │
│  └──────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
```

### Sections

| Section | Purpose |
|---------|---------|
| **Transport bar** | Play, stop, record, tempo, time sig, position |
| **Arrangement** | Tracks, clips, timeline - the visual music |
| **Track strip** | Selected track's plugins + mixer (inline) |
| **Chat panel** | AI conversation with inline audio/visual feedback |

### Key Shortcuts

| Key | Action |
|-----|--------|
| `Space` | Play / Stop |
| `R` | Record |
| `Cmd+Enter` | Send chat message |
| `Cmd+/` | Toggle full-screen chat |
| `Cmd+1` | Focus arrangement |
| `Cmd+2` | Focus chat |
| `Tab` | Cycle focus: arrangement → track strip → chat |
| `Cmd+Z` | Undo (works on AI actions too) |

---

## Chat Panel

### Position & Behavior

- **Default**: Bottom panel, ~25% of window height
- **Resizable**: Drag to resize
- **Full-screen**: `Cmd+/` toggles full chat mode (arrangement minimized)
- **Collapsible**: Can minimize to just input bar

### Inline Audio in Chat

AI responses can include embedded audio/visual elements:

```
AI: Created an 8-bar drum pattern:

    ┌────────────────────────────────────────┐
    │  ▶   Drum Pattern         0:00 / 0:16 │
    │  ░░░░████░░░░████░░░░████░░░░████░░░░ │
    └────────────────────────────────────────┘

    [Keep] [Regenerate] [Edit]

AI: Here's the MIDI if you want to tweak it:

    ┌────────────────────────────────────────┐
    │ C2 │●   ●   ●   ●   ●   ●   ●   ●    │ Kick
    │ D2 │  ●   ●   ●   ●   ●   ●   ●   ●  │ Snare
    │ F#2│●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●●  │ Hi-hat
    └────────────────────────────────────────┘

    [Open in editor] [Adjust velocity]
```

### Context Awareness

The chat always knows:
- Currently selected track(s)
- Current playhead position
- What's playing
- Recent actions

```
You: (with "Vocals" track selected)
> Add some compression

AI: Added Compressor to Vocals. Threshold at -18dB, ratio 4:1.
```

No need to say "add compression to the vocals track" - selection provides context.

---

## Arrangement View

### Track Lanes

Each track shows:
- Name (editable)
- [E] Expand - show/hide plugin chain
- [M] Mute
- [S] Solo
- Clips as waveforms (audio) or blocks (MIDI)

### Clips

**Audio clips**: Show waveform
**MIDI clips**: Show note density or mini piano roll

```
Audio clip:          MIDI clip:
┌──────────────┐    ┌──────────────┐
│ ∿∿∿∿∿∿∿∿∿∿∿∿ │    │ ▪ ▪▪  ▪ ▪▪  │
└──────────────┘    │▪ ▪  ▪▪ ▪  ▪▪│
                    └──────────────┘
```

### Selection

- Click track → track selected (context for chat)
- Click clip → clip selected
- Drag → select time range
- Cmd+click → multi-select

---

## Track Strip (Selected Track)

Shows for currently selected track:

```
┌─ Vocals ───────────────────────────────────────────────────────────────┐
│                                                                        │
│  [+]  EQ → Compressor → De-esser → Reverb (send)     ═══●═══  -3dB ██▓│
│       ↑      ↑           ↑          ↑                                  │
│     click to open plugin UI                          vol    meter     │
└────────────────────────────────────────────────────────────────────────┘
```

### Plugin Chain Display

- Shows plugins in signal flow order (critical for understanding)
- Click plugin name → opens floating window
- Drag to reorder
- Right-click for bypass/remove

---

## Plugin Windows

### Floating Windows

- Plugin UIs open as **floating windows**
- Can be closed without removing plugin
- Position remembers where you left them
- Multiple plugins can be open simultaneously

```
┌─ Compressor (Vocals) ─────────────────────── ○ ○ ●  ─┐
│                                                      │
│     ┌─────┐    ┌─────┐    ┌─────┐    ┌─────┐       │
│     │     │    │     │    │     │    │     │       │
│     │  T  │    │  R  │    │  A  │    │  R  │       │
│     │     │    │     │    │     │    │     │       │
│     └──●──┘    └──●──┘    └──●──┘    └──●──┘       │
│    Threshold   Ratio     Attack    Release         │
│     -18 dB     4:1       10 ms     100 ms          │
│                                                      │
│    [Presets ▼]              [Bypass] [Remove]       │
└──────────────────────────────────────────────────────┘
```

### AI + Plugin Interaction

When a plugin is open, you can reference it naturally:

```
You: Lower the threshold a bit
AI: Lowered Compressor threshold to -24dB.

You: Show me the attack options
AI: Current attack is 10ms. Range is 0.1ms to 200ms.
    For vocals, try 5-15ms for natural sound, <5ms for aggressive.
    [Set to 5ms] [Set to 15ms]
```

---

## MIDI

### First-Class Support

MIDI is not an afterthought:

- **MIDI tracks**: Record/play MIDI to virtual instruments
- **MIDI input**: External controllers, keyboards
- **MIDI editor**: Piano roll for detailed editing
- **AI-generated MIDI**: Drum patterns, chord progressions, melodies

### MIDI Editor (Piano Roll)

Opens when editing a MIDI clip:

```
┌─ MIDI Editor: Drum Pattern ──────────────────────────────────────────┐
│     1       2       3       4       5       6       7       8       │
│     ├───────┼───────┼───────┼───────┼───────┼───────┼───────┼───    │
│ C#2 │                                                                │
│ C2  │●       ●       ●       ●       ●       ●       ●       ●      │ Kick
│ B1  │                                                                │
│ A#1 │    ●       ●       ●       ●       ●       ●       ●       ●  │ Snare
│ A1  │                                                                │
│ ...                                                                  │
├──────────────────────────────────────────────────────────────────────┤
│ Velocity: ████████░░  │  Length: 1/4  │  Snap: 1/16                 │
└──────────────────────────────────────────────────────────────────────┘
```

### AI + MIDI

```
You: Make the hi-hats more interesting
AI: Added syncopation and velocity variation:

    Before: ●●●●●●●●●●●●●●●●
    After:  ●○●●○●●○●●○●●○●●  (○ = softer hits)

    [Preview] [Apply] [More variation]
```

---

## Views & Modes

### Primary Views

| View | Purpose | Shortcut |
|------|---------|----------|
| **Arrange** | Full arrangement + chat | Default |
| **Mix** | All tracks as mixer channels + chat | `Cmd+M` |
| **Edit** | MIDI/audio editor + chat | Double-click clip |

### Chat Modes

| Mode | Description | Shortcut |
|------|-------------|----------|
| **Docked** | Bottom panel (default) | - |
| **Full** | Chat fills window, mini-arrangement at top | `Cmd+/` |
| **Minimal** | Just input bar, expands on focus | `Cmd+Shift+/` |

---

## Mixer View

When mixing is the focus:

```
┌─────────────────────────────────────────────────────────────────────────┐
│  ▶  ⏹  ⏺  │  ♩ 120  │  4/4  │  1:32.4  │               [⚙]  [◱]      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐    │
│  │ Drums  │ │ Bass   │ │ Vocals │ │ Synth  │ │ Guitar │ │ Master │    │
│  ├────────┤ ├────────┤ ├────────┤ ├────────┤ ├────────┤ ├────────┤    │
│  │   EQ   │ │   EQ   │ │  Comp  │ │  Dist  │ │  Amp   │ │  Limit │    │
│  │  Comp  │ │  Comp  │ │  EQ    │ │  Delay │ │  EQ    │ │  EQ    │    │
│  │  Verb  │ │        │ │  Verb  │ │  Verb  │ │  Verb  │ │        │    │
│  ├────────┤ ├────────┤ ├────────┤ ├────────┤ ├────────┤ ├────────┤    │
│  │ ◀━●━▶  │ │ ◀━●━▶  │ │ ◀━●━▶  │ │ ◀━●━▶  │ │ ◀━●━▶  │ │        │    │
│  │        │ │        │ │        │ │        │ │        │ │        │    │
│  │   ▓    │ │   ▓    │ │   █    │ │   ▓    │ │   ▓    │ │   █    │    │
│  │   █    │ │   █    │ │   █    │ │   █    │ │   █    │ │   █    │    │
│  │   █    │ │   █    │ │   █    │ │   █    │ │   █    │ │   █    │    │
│  │  ━●━   │ │  ━●━   │ │  ━●━   │ │  ━●━   │ │  ━●━   │ │  ━●━   │    │
│  │ [M][S] │ │ [M][S] │ │ [M][S] │ │ [M][S] │ │ [M][S] │ │        │    │
│  └────────┘ └────────┘ └────────┘ └────────┘ └────────┘ └────────┘    │
│                                                                         │
├─────────────────────────────────────────────────────────────────────────┤
│ You: The bass is fighting with the kick, how do I fix that?            │
│                                                                         │
│ AI: Classic low-end clash. Options:                                     │
│ 1. Sidechain the bass to duck when kick hits [Apply]                   │
│ 2. Cut 60Hz on bass, boost on kick (frequency split) [Apply]           │
│ 3. Let me analyze and suggest specific frequencies [Analyze]           │
├─────────────────────────────────────────────────────────────────────────┤
│ > _                                                                     │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Visual Design

### Colors (Dark Theme)

| Element | Color | Hex |
|---------|-------|-----|
| Background | Dark gray | `#1A1A1A` |
| Panels | Medium gray | `#2D2D2D` |
| Track lanes | Alternating | `#252525` / `#2A2A2A` |
| Accent | Blue | `#4A9EFF` |
| Recording | Red | `#FF4A4A` |
| Text primary | White | `#FFFFFF` |
| Text secondary | Gray | `#888888` |
| Waveforms | Accent blue | `#4A9EFF` |
| MIDI notes | Accent blue | `#4A9EFF` |
| Meters | Green → Yellow → Red | Gradient |

### Typography

- **UI**: SF Pro (system) or Inter
- **Monospace** (time, BPM): SF Mono or JetBrains Mono
- **Chat**: Same as UI, slightly larger for readability

---

## AI-First Mixer Controls

Instead of complex automation lanes:

```
You: Automate the filter on the synth to open up during the chorus

AI: Added filter automation to Synth:
    - Verse: Filter at 800Hz (muted/dark)
    - Pre-chorus: Sweep from 800Hz to 2kHz over 4 bars
    - Chorus: Wide open at 8kHz

    ┌────────────────────────────────────────┐
    │         ╱‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾╲           │
    │    ╱‾‾‾‾                    ╲____     │
    │___╱                                    │
    │ Verse    Pre     Chorus    Verse      │
    └────────────────────────────────────────┘

    [Preview] [Adjust] [Clear automation]
```

Instead of drawing, you describe. The AI shows what it did visually.

---

## Questions Still Open

1. **Browser panel?** - For files, plugins, presets. Or is search/chat enough?
2. **Undo history UI?** - Show list of actions? Or just Cmd+Z?
3. **Multiple windows?** - One window only? Or allow arrangement + mixer split?
4. **Touch/tablet?** - Future consideration for iPad?

---

## Summary

| Traditional DAW | DAIW |
|-----------------|------|
| Toolbar with 50 buttons | Transport + chat |
| Right-click menus everywhere | Natural language |
| Automation lanes to draw | Describe the movement |
| Complex routing dialogs | "Send this to that" |
| Preferences with 500 options | Sensible defaults + chat |
| Multiple floating panels | Arrangement + track strip + chat |
| Plugin as tiny insert slot | Plugin chain clearly visible |
| MIDI as separate mode | MIDI integrated naturally |

**The goal**: Someone who's never used a DAW can make music. Someone who's used DAWs for 20 years feels at home.
