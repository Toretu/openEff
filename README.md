# OpenGuitar - Guitar Effects Plugins

A collection of VST3 guitar effect plugins built with JUCE.

[![Build Status](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/build.yml/badge.svg)](https://github.com/YOUR_USERNAME/YOUR_REPO/actions)

## Features

### Included Plugins

1. **OpenGuitar Fuzz** - Vintage fuzz distortion pedal
2. **OpenGuitar Compressor** - Dynamic range compression
3. **OpenGuitar Reverb** - Spatial reverb effect

### Common Features

- **Vintage Sound**: Emulates classic guitar pedal characteristics
- **Bypass**: True bypass switching for each effect
- **Oversampling**: Anti-aliasing for clean distortion tones
- **VST3 Format**: Compatible with all major DAWs (Reaper, Ableton, FL Studio, Pro Tools, etc.)
- **Cross-Platform**: Works on Windows, macOS, and Linux

### Fuzz Pedal
- **Gain**: Controls the amount of distortion (0-10)
- **Tone**: Filters the distorted signal (300Hz - 5kHz)
- **Level**: Output volume control
- Asymmetric clipping for classic fuzz character

## Download Pre-Built Plugins

**[Download the latest release](https://github.com/YOUR_USERNAME/YOUR_REPO/releases/latest)**

Download the appropriate ZIP file for your operating system and follow the installation instructions included in the release.

## Building from Source

For detailed build instructions, see [BUILDING.md](BUILDING.md).

### Quick Start

### Prerequisites

1. [CMake](https://cmake.org/) (3.15 or later)
2. C++17 compatible compiler (MSVC, GCC, or Clang)
3. [JUCE Framework](https://juce.com/) (v7.x)

### Setup JUCE

Clone JUCE into the project directory:

```bash
cd c:\code\OpenGuitar
git clone https://github.com/juce-framework/JUCE.git
```

### Build Instructions (Windows)

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The VST3 plugin will be output to `build/OpenGuitar_artefacts/Release/VST3/`

### Installation

Copy the `.vst3` folder to your DAW's VST3 plugin directory:
- **Windows**: `C:\Program Files\Common Files\VST3\`
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/` or `/Library/Audio/Plug-Ins/VST3/`
- **Linux**: `~/.vst3/` or `/usr/lib/vst3/`

Then rescan plugins in your DAW.

## Usage

1. Load the plugin in your DAW on a guitar track
2. Adjust the **Gain** knob to set the fuzz amount
3. Use the **Tone** knob to shape the frequency response
4. Set the **Level** knob to control output volume
5. Click **Bypass** to toggle the effect on/off

## Technical Details

- **Sample Rate**: Supports 44.1kHz - 192kHz
- **Processing**: 32-bit floating point
- **Latency**: Minimal (<5ms due to oversampling)
- **CPU Usage**: Low (optimized DSP)

## Future Enhancements

- Additional effect pedals (Delay, Chorus, Overdrive, Tremolo, Phaser)
- Effect chaining system
- Preset management and factory presets
- Visual pedal board interface with drag-and-drop
- MIDI control support and MIDI learn
- Cabinet simulation / IR loader

## License

See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

### Creating a Release

For maintainers, to create a new release:

**Linux/macOS:**
```bash
./release.sh
```

**Windows:**
```bash
release.bat
```

Or manually:
```bash
git tag v1.0.0
git push origin v1.0.0
```

GitHub Actions will automatically build the plugins for all platforms and create a release with downloadable files.
