# Auto-Increment Version System

## Overview
OpenGuitar now automatically increments the version number on each CMake configuration. The version follows semantic versioning format: `MAJOR.MINOR.PATCH`

## How It Works

1. **Version Storage**: The current version is stored in `version.txt` at the project root
2. **Auto-Increment**: Each time you run `cmake -B build` or reconfigure CMake, the PATCH version is automatically incremented
3. **Build Version**: The current build uses the version from `version.txt` before incrementing

## Version Format
- **MAJOR**: Manually set in `version.txt` for major releases (breaking changes)
- **MINOR**: Manually set in `version.txt` for minor releases (new features)
- **PATCH**: Auto-incremented on each CMake configuration

## Example Workflow

```powershell
# Initial version: 1.0.15
cmake -B build
# Builds as: 1.0.15
# Updates version.txt to: 1.0.16

cmake --build build --config Release
# Builds plugins with version 1.0.15

# Reconfigure (triggers increment)
cmake -B build
# Builds as: 1.0.16
# Updates version.txt to: 1.0.17
```

## Manually Changing Version

To change the major or minor version, simply edit `version.txt`:

```bash
echo 2.0.0 > version.txt
```

The next CMake configuration will use `2.0.0` and increment to `2.0.1`.

## Build Messages

When configuring, you'll see:
```
-- Building OpenGuitar version 1.0.16
-- Next build will be version 1.0.17
```

This tells you:
- Current build version: `1.0.16`
- Next reconfiguration will use: `1.0.17`

## Notes

- The version file is tracked in git, so team members see version changes
- Each reconfigure increments the version (not each build)
- To build without incrementing, just run `cmake --build build` without reconfiguring
- Version is embedded in all VST3 plugins automatically
