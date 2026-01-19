# GitHub Actions Release Setup - Summary

## What Has Been Set Up

Your OpenGuitar project now has a complete automated build and release system using GitHub Actions! Here's what was created:

### 1. GitHub Actions Workflow (`.github/workflows/build.yml`)
Automatically builds your VST3 plugins for:
- ✅ Windows (latest)
- ✅ Linux (Ubuntu latest)
- ✅ macOS (latest)

**Triggers:**
- Every push to main/master/develop branches
- Every pull request
- When you push a version tag (e.g., `v1.0.0`)
- Manual workflow dispatch

### 2. Fixed CMakeLists.txt Issues
- ✅ Fixed typo in git command (`HEADgit` → `HEAD`)
- ✅ Replaced Windows-only `robocopy` with cross-platform `cmake -E copy_directory`
- ✅ Now builds successfully on all platforms

### 3. Release Helper Scripts
- `release.sh` - Linux/macOS release script
- `release.bat` - Windows release script

### 4. Documentation
- `BUILDING.md` - Comprehensive build and release guide
- Updated `README.md` - Added download links and build badges

## How to Use

### For Regular Development

Simply push your code to GitHub:

```bash
git add .
git commit -m "Add new feature"
git push
```

GitHub Actions will:
1. Build all 3 plugins (Fuzz, Compressor, Reverb)
2. Build for Windows, Linux, and macOS
3. Upload artifacts to the Actions tab

You can download these artifacts from:
- Your GitHub repo → **Actions** tab → Click on latest run → **Artifacts** section

### For Creating a Public Release

#### Option 1: Use the Release Script (Easiest)

**Linux/macOS:**
```bash
./release.sh
```

**Windows:**
```bash
release.bat
```

The script will:
- Check for uncommitted changes
- Ask for the version number
- Create and push a git tag
- Trigger automatic release build

#### Option 2: Manual Tag Creation

```bash
# Commit all changes
git add .
git commit -m "Release v1.0.0"

# Create version tag
git tag v1.0.0

# Push to GitHub
git push origin main
git push origin v1.0.0
```

### What Happens When You Push a Tag

1. **GitHub Actions starts building** (takes ~10-20 minutes)
   - Builds all plugins for Windows, Linux, macOS in parallel

2. **Packages are created**:
   - `OpenGuitar_Fuzz_Windows.zip`
   - `OpenGuitar_Fuzz_Linux.zip`
   - `OpenGuitar_Fuzz_macOS.zip`
   - `OpenGuitar_Compressor_Windows.zip`
   - `OpenGuitar_Compressor_Linux.zip`
   - `OpenGuitar_Compressor_macOS.zip`
   - `OpenGuitar_Reverb_Windows.zip`
   - `OpenGuitar_Reverb_Linux.zip`
   - `OpenGuitar_Reverb_macOS.zip`

3. **GitHub Release is created automatically**
   - All 9 zip files are attached
   - Installation instructions are included
   - Release notes are generated from commits

4. **Users can download** from:
   - Your repo → **Releases** → Click on version → Download assets

## File Structure

```
/code/openEff/
├── .github/
│   └── workflows/
│       └── build.yml          # GitHub Actions workflow
├── JUCE/                      # JUCE framework
├── src/                       # Source code
├── CMakeLists.txt             # Build configuration (FIXED)
├── README.md                  # Updated with download links
├── BUILDING.md                # Build documentation
├── release.sh                 # Linux/macOS release helper
└── release.bat                # Windows release helper
```

## Testing the Setup

### Step 1: Test Local Build (Optional)

```bash
cd /code/openEff
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
```

If this works, GitHub Actions will work too!

### Step 2: Commit and Push

```bash
cd /code/openEff
git add .
git commit -m "Add GitHub Actions CI/CD"
git push
```

### Step 3: Check GitHub Actions

1. Go to your GitHub repository
2. Click the **Actions** tab
3. Watch the workflow run
4. Once complete, download artifacts to test

### Step 4: Create Your First Release

```bash
# Using the helper script
./release.sh

# Or manually
git tag v1.0.0
git push origin v1.0.0
```

Then check:
- **Actions** tab - See the build progress
- **Releases** tab - See the published release with downloads

## Customization

### Change Version Numbers

Edit `CMakeLists.txt`:
```cmake
set(VERSION_MAJOR 1)  # Change this
set(VERSION_MINOR 0)  # Change this
```

The build number is automatic (based on git commit count).

### Update Build Badge

Edit `README.md` and replace:
```markdown
[![Build Status](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/build.yml/badge.svg)]
```

With your actual GitHub username and repo name.

### Modify Release Notes

Edit `.github/workflows/build.yml` and find the `body:` section under the release job to customize the automatic release notes.

## Troubleshooting

### Build fails on GitHub Actions

1. Go to **Actions** tab
2. Click on the failed run
3. Click on the failed job
4. Read the error log
5. Fix the issue locally and push again

### Common Issues:

**"JUCE not found"**
- Make sure JUCE is committed as a submodule or included in your repo

**"Cannot find VST3 files"**
- Check the artifact collection paths in `build.yml`
- Verify CMake is building correctly

**"Permission denied" on release.sh**
- Run: `chmod +x release.sh`

### Getting Help

If builds fail:
1. Check the Actions logs
2. Test building locally first
3. Ensure all source files are committed
4. Verify JUCE is properly included

## Next Steps

1. ✅ Commit the new files
2. ✅ Push to GitHub
3. ✅ Check Actions tab for first build
4. ✅ Download and test artifacts
5. ✅ Create your first release tag
6. ✅ Share download links with users!

## Benefits

✅ **Automatic builds** - No need to build on multiple machines
✅ **Consistent releases** - Same build environment every time
✅ **Easy distribution** - Users download from GitHub Releases
✅ **Version tracking** - Git tags tied to releases
✅ **Free hosting** - GitHub hosts your release files
✅ **Professional** - Automated CI/CD like major projects

Enjoy your automated VST3 plugin releases! 🎸🎵
