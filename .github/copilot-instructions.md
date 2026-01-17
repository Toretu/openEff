# Oke
penGuitar - Guitar Effects Plugin

## Project Overview
OpenGuitar is a VST3 guitar effects plugin that emulates classic guitar effect pedals including compressor, fuzz, delay, chorus, and other common effects. The plugin allows users to select different effect pedals, chain them together, and save/load presets for use in digital audio workstations (DAWs).

## Core Requirements

### Plugin Format
- **Target Format**: VST3 (using JUCE framework or similar)
- **Compatibility**: Windows, macOS, and Linux
- **Sample Rates**: Support 44.1kHz, 48kHz, 88.2kHz, 96kHz, and higher
- **Bit Depth**: 32-bit floating point processing

### Effect Pedals to Emulate
1. **Compressor** - Dynamic range compression with attack, release, ratio, threshold controls
2. **Fuzz** - Classic fuzz distortion with tone and gain controls
3. **Delay** - Time-based echo effect with feedback, time, and mix controls
4. **Chorus** - Modulation effect with rate, depth, and mix controls
5. **Overdrive** - Tube-like overdrive with gain, tone, and level controls
6. **Reverb** - Spatial effect with room size, damping, and mix controls
7. **Tremolo** - Amplitude modulation with rate and depth controls
8. **Phaser** - Phase shifting effect with rate, depth, and feedback controls

### Key Features
- **Effect Chain**: Ability to add, remove, and reorder effects in a signal chain
- **Preset Management**: Load and save user presets with all effect parameters
- **Bypass**: Individual bypass for each effect and global bypass
- **Parameter Automation**: All parameters should be automatable in DAWs
- **Low Latency**: Minimal processing latency for real-time performance

## Technical Architecture

### Framework
- Use **JUCE** framework for cross-platform VST3 development
- Alternative: **iPlug2** for lightweight implementation

### Audio Processing
- Process audio in mono or stereo
- Use buffered processing for efficiency
- Implement proper anti-aliasing where needed (especially for distortion/fuzz)
- Use oversampling for non-linear effects to reduce aliasing

### Code Structure
```
src/
  ├── effects/           # Individual effect implementations
  │   ├── Compressor.cpp/h
  │   ├── Fuzz.cpp/h
  │   ├── Delay.cpp/h
  │   ├── Chorus.cpp/h
  │   └── ...
  ├── dsp/              # DSP utilities and algorithms
  │   ├── Filter.cpp/h
  │   ├── Oscillator.cpp/h
  │   └── DelayLine.cpp/h
  ├── ui/               # User interface components
  │   ├── PedalComponent.cpp/h
  │   ├── EffectChainView.cpp/h
  │   └── ...
  ├── PluginProcessor.cpp/h  # Main audio processor
  └── PluginEditor.cpp/h     # Main UI editor
```

### Effect Base Class
Each effect should inherit from a common base class with:
- `processBlock()` - Process audio buffer
- `setParameter()` - Update effect parameters
- `getParameter()` - Get current parameter values
- `bypass()` - Bypass effect
- `reset()` - Reset internal state

### Preset System
- Store presets as JSON or XML files
- Include effect chain order and all parameters
- Support factory presets and user presets
- Preset browser in UI

## UI/UX Guidelines

### Visual Design
- Skeuomorphic pedal design (resembling real guitar pedals)
- Each effect has distinct visual appearance matching real-world counterparts
- Drag-and-drop to reorder effects in chain
- LED indicators for bypass status
- Knobs and switches matching real pedal controls

### Controls
- Rotary knobs for continuous parameters (gain, tone, mix, etc.)
- Toggle switches for bypass and mode switches
- Visual feedback for parameter changes
- MIDI learn capability for external control

## DSP Implementation Notes

### Compressor
- Use RMS or peak detection
- Smooth gain reduction curve
- Attack/release envelope followers
- Optional auto-makeup gain

### Fuzz
- Asymmetric clipping for vintage fuzz tone
- Pre-filter and post-filter for tone shaping
- Consider oversampling to reduce aliasing artifacts

### Delay
- Circular buffer implementation
- Interpolated delay line for smooth modulation
- Feedback path with proper gain staging
- Optional low-pass/high-pass filters in feedback

### Chorus
- Use LFO with sine/triangle wave
- Multiple delay lines for richer chorus
- Interpolation for smooth modulation
- Stereo widening option

## Performance Considerations
- Optimize for real-time audio processing
- Use SIMD instructions where applicable (SSE, AVX, NEON)
- Minimize allocations in audio thread
- Use lock-free data structures for parameter updates
- Profile and optimize hot paths

## Testing Requirements
- Unit tests for individual DSP algorithms
- Integration tests for effect chain
- Test with various sample rates and buffer sizes
- Validate preset save/load functionality
- Test automation in popular DAWs (Reaper, Ableton, FL Studio, Pro Tools)

## Build System
- Use CMake for cross-platform builds
- Support for major compilers (MSVC, GCC, Clang)
- Automated builds for Windows, macOS, Linux
- Include installer generation

## Documentation
- API documentation for effect classes
- User manual with effect descriptions
- Parameter ranges and suggested settings
- Known issues and limitations

## Code Style
- Follow JUCE coding conventions
- Use meaningful variable and function names
- Comment complex DSP algorithms
- Keep functions focused and concise
- Use const correctness

## Dependencies
- JUCE framework (version 7.x or later)
- Standard C++ library (C++17 or later)
- Optional: FFTW for frequency-domain processing

## Future Enhancements
- MIDI control for effect switching
- Tuner and noise gate
- Cabinet simulation/IR loader
- Stereo effects and dual signal paths
- Modulation matrix for advanced routing
- Undo/redo for preset editing

## Important Notes for Development
- Always test with real guitar input, not just test signals
- Pay attention to input/output gain staging to prevent clipping
- Consider CPU usage - guitar players often use multiple instances
- Ensure parameters respond musically (logarithmic scaling for frequencies, etc.)
- Test with both single-coil and humbucker pickups
- Validate that presets recall correctly across different sessions
