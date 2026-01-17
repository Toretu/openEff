# OpenGuitar - Fuzz Pedal

A VST3 guitar fuzz effect plugin built with JUCE.

## Features

- **Vintage Fuzz Sound**: Asymmetric clipping for classic fuzz character
- **Three Controls**:
  - **Gain**: Controls the amount of distortion (0-10)
  - **Tone**: Filters the distorted signal (300Hz - 5kHz)
  - **Level**: Output volume control
- **Bypass**: True bypass switching
- **Oversampling**: 2x oversampling to reduce aliasing artifacts
- **VST3 Format**: Compatible with all major DAWs

## Building

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

Copy the `.vst3` file to your DAW's VST3 plugin folder:
- **Windows**: `C:\Program Files\Common Files\VST3\`
- **macOS**: `~/Library/Audio/Plug-Ins/VST3/`
- **Linux**: `~/.vst3/`

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

- Additional effect pedals (Delay, Chorus, Compressor, etc.)
- Effect chaining
- Preset management
- Visual pedal board interface
- MIDI control support

## License

See LICENSE file for details.
