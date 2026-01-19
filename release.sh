#!/bin/bash

# Quick Release Script for OpenGuitar VST3 Plugins
# This script helps you create a new release version

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}OpenGuitar VST3 Release Helper${NC}"
echo "================================"
echo ""

# Check if git repo is clean
if [[ -n $(git status -s) ]]; then
    echo -e "${YELLOW}Warning: You have uncommitted changes.${NC}"
    echo "Current changes:"
    git status -s
    echo ""
    read -p "Do you want to commit them now? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        read -p "Enter commit message: " commit_msg
        git add .
        git commit -m "$commit_msg"
    else
        echo -e "${RED}Please commit your changes before creating a release.${NC}"
        exit 1
    fi
fi

# Get current version from git tags
current_version=$(git describe --tags --abbrev=0 2>/dev/null || echo "v0.0.0")
echo -e "Current version: ${GREEN}$current_version${NC}"
echo ""

# Ask for new version
read -p "Enter new version (e.g., v1.0.0): " new_version

# Validate version format
if [[ ! $new_version =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo -e "${RED}Error: Version must be in format vX.Y.Z (e.g., v1.0.0)${NC}"
    exit 1
fi

# Optional release notes
echo ""
echo "Enter release notes (optional, press Ctrl+D when done):"
release_notes=$(cat)

# Confirmation
echo ""
echo -e "${YELLOW}Ready to create release:${NC}"
echo "  Version: $new_version"
echo "  Current branch: $(git branch --show-current)"
echo ""
read -p "Continue? (y/n) " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Release cancelled."
    exit 0
fi

# Create and push tag
echo -e "${GREEN}Creating tag $new_version...${NC}"
if [[ -n $release_notes ]]; then
    git tag -a "$new_version" -m "$release_notes"
else
    git tag -a "$new_version" -m "Release $new_version"
fi

echo -e "${GREEN}Pushing to GitHub...${NC}"
git push origin $(git branch --show-current)
git push origin "$new_version"

echo ""
echo -e "${GREEN}✓ Release $new_version created successfully!${NC}"
echo ""
echo "GitHub Actions is now building your plugins for Windows, Linux, and macOS."
echo "Check the progress at: https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]\(.*\)\.git/\1/')/actions"
echo ""
echo "Once the build completes, the release will be available at:"
echo "https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]\(.*\)\.git/\1/')/releases/tag/$new_version"
