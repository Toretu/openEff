# Building and Releasing OpenGuitar VST3 Plugins

This document explains how to build and release the OpenGuitar VST3 plugins using GitHub Actions.

## Automated Builds (GitHub Actions)

Every time you push to the repository, GitHub Actions will automatically:
- Build all VST3 plugins for **Windows**, **Linux**, and **macOS**
- Upload the builds as artifacts that you can download

### Viewing Build Artifacts

1. Go to the **Actions** tab in your GitHub repository
2. Click on the latest workflow run
3. Scroll down to **Artifacts** section
4. Download the zip files for each platform:
   - `OpenGuitar-VST3-Linux`
   - `OpenGuitar-VST3-Windows`
   - `OpenGuitar-VST3-macOS`

## Creating a Release

To create a public release with downloadable VST3 files:

### 1. Tag a Release Version

```bash
# Make sure all changes are committed
git add .
git commit -m "Release v1.0.0"

# Create and push a version tag
git tag v1.0.0
git push origin v1.0.0
```

### 2. Automatic Release Creation

Once you push a tag starting with `v` (e.g., `v1.0.0`, `v2.1.3`), GitHub Actions will:
- Build all plugins for all platforms
- Create a GitHub Release automatically
- Attach all VST3 zip files to the release
- Add installation instructions

### 3. View the Release

1. Go to the **Releases** section of your GitHub repository
2. Your new release will be available with downloadable files:
   - `OpenGuitar_Fuzz_Windows.zip`
   - `OpenGuitar_Fuzz_Linux.zip`
   - `OpenGuitar_Fuzz_macOS.zip`
   - `OpenGuitar_Compressor_Windows.zip`
   - `OpenGuitar_Compressor_Linux.zip`
   - `OpenGuitar_Compressor_macOS.zip`
   - `OpenGuitar_Reverb_Windows.zip`
   - `OpenGuitar_Reverb_Linux.zip`
   - `OpenGuitar_Reverb_macOS.zip`

## Building Locally

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
# Note: Use libwebkit2gtk-4.1-dev on Ubuntu 24.04+, or libwebkit2gtk-4.0-dev on older versions
sudo apt-get install -y \
    libasound2-dev \
    libcurl4-openssl-dev \
    libfreetype6-dev \
    libx11-dev \
    libxcomposite-dev \
    libxcursor-dev \
    libxext-dev \
    libxinerama-dev \
    libxrandr-dev \
    libxrender-dev \
    libwebkit2gtk-4.1-dev \
    libglu1-mesa-dev \
    mesa-common-dev

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

# VST3 files will be in: build/*_artefacts/Release/VST3/
```

### Windows

```powershell
# Install CMake and Visual Studio 2019 or later
# Then build:
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

# VST3 files will be in: build\*_artefacts\Release\VST3\
```

### macOS

```bash
# Install Xcode and CMake
# Then build:
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

# VST3 files will be in: build/*_artefacts/Release/VST3/
```

## Installation Instructions for End Users

### Windows
1. Extract the ZIP file
2. Copy the `.vst3` folder to: `C:\Program Files\Common Files\VST3\`
3. Rescan plugins in your DAW

### macOS
1. Extract the ZIP file
2. Copy the `.vst3` bundle to: `~/Library/Audio/Plug-Ins/VST3/` (user) or `/Library/Audio/Plug-Ins/VST3/` (system)
3. Rescan plugins in your DAW

### Linux
1. Extract the ZIP file
2. Copy the `.vst3` folder to: `~/.vst3/` or `/usr/lib/vst3/`
3. Rescan plugins in your DAW

## Troubleshooting

### Build fails on GitHub Actions
- Check the **Actions** tab for error logs
- Ensure all source files are committed
- Verify JUCE submodule is properly initialized

### VST3 not found by DAW
- Make sure you copied to the correct directory
- Try rescanning plugins in your DAW
- Check that the VST3 folder structure is preserved (it should be a folder, not individual files)

### Version number is 1.0.0
- This is based on git commit count
- Push your code to get proper version numbers
- Format: `Major.Minor.CommitCount`

## Versioning

The project uses semantic versioning based on:
- **Major**: Set in CMakeLists.txt (currently 1)
- **Minor**: Set in CMakeLists.txt (currently 0)
- **Build**: Automatic from git commit count

To change the version:
1. Edit `CMakeLists.txt`
2. Update `VERSION_MAJOR` and `VERSION_MINOR`
3. Commit and create a new tag
