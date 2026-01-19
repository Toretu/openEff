# Quick Start Checklist

Follow these steps to get your automated releases working:

## 1. Initial Setup ✓

Files have been created:
- [x] `.github/workflows/build.yml` - GitHub Actions workflow
- [x] `BUILDING.md` - Build documentation
- [x] `RELEASE_SETUP.md` - Complete setup guide
- [x] `release.sh` - Linux/macOS release script
- [x] `release.bat` - Windows release script
- [x] `CMakeLists.txt` - Fixed for cross-platform builds

## 2. Commit to GitHub

```bash
cd /code/openEff
git add .
git commit -m "Add GitHub Actions CI/CD for automated releases"
git push origin main  # or 'master' depending on your default branch
```

## 3. Verify First Build

1. Go to: https://github.com/YOUR_USERNAME/YOUR_REPO/actions
2. Wait for the build to complete (~10-20 minutes)
3. Check that all 3 jobs succeed (Windows, Linux, macOS)
4. Download artifacts and test the VST3 files

## 4. Create Your First Release

**Using the script:**
```bash
./release.sh
```
Enter version: `v1.0.0`

**Or manually:**
```bash
git tag v1.0.0
git push origin v1.0.0
```

## 5. Verify Release

1. Go to: https://github.com/YOUR_USERNAME/YOUR_REPO/actions
2. Wait for release build to complete
3. Go to: https://github.com/YOUR_USERNAME/YOUR_REPO/releases
4. Download and test the VST3 zip files

## 6. Update README Badge (Optional)

Edit `README.md` line 5, replace:
```
YOUR_USERNAME/YOUR_REPO
```
with your actual GitHub username and repository name.

## 7. Share with Users!

Users can now download from:
https://github.com/YOUR_USERNAME/YOUR_REPO/releases/latest

## Troubleshooting

If anything fails, see:
- `RELEASE_SETUP.md` - Full troubleshooting guide
- `BUILDING.md` - Build instructions
- GitHub Actions logs - Click on failed job to see errors

## Future Releases

Every time you want to create a new release:

1. Make changes
2. Commit: `git commit -am "Your changes"`
3. Push: `git push`
4. Release: `./release.sh` (enter new version like `v1.0.1`)
5. Done! GitHub Actions handles the rest.
