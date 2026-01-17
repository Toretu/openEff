cmake_minimum_required(VERSION 3.15)

# Get version from Git
execute_process(
    COMMAND git rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Fallback if not in a git repo
if(NOT GIT_COMMIT_COUNT)
    set(GIT_COMMIT_COUNT 0)
endif()

# Version: Major.Minor.BuildNumber
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_BUILD ${GIT_COMMIT_COUNT})

project(OpenGuitar VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD})

message(STATUS "Building OpenGuitar version ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
add_subdirectory(JUCE)

# Create the plugin target
juce_add_plugin(OpenGuitar
    COMPANY_NAME "OpenGuitar"
    PLUGIN_MANUFACTURER_CODE Ogtr
    PLUGIN_CODE Ogfz
    FORMATS VST3
    PRODUCT_NAME "OpenGuitar Fuzz"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Fx" "Distortion")

target_sources(OpenGuitar
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginProcessor.h
        src/PluginEditor.cpp
        src/PluginEditor.h
        src/effects/Compressor.cpp
        src/effects/Compressor.h
        src/effects/Fuzz.cpp
        src/effects/Fuzz.h
        src/dsp/Filter.cpp
        src/dsp/Filter.h)

target_compile_definitions(OpenGuitar
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

target_link_libraries(OpenGuitar
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
