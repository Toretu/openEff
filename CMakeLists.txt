cmake_minimum_required(VERSION 3.15)

# Get version from Git
execute_process(
    COMMAND git rev-list --count HEADgit 
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

# Fuzz Plugin
juce_add_plugin(OpenGuitar_Fuzz
    COMPANY_NAME "OpenGuitar"
    PLUGIN_MANUFACTURER_CODE Ogtr
    PLUGIN_CODE Ogfz
    FORMATS VST3
    PRODUCT_NAME "OpenGuitar Fuzz"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx" "Distortion")

target_sources(OpenGuitar_Fuzz
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginProcessor.h
        src/PluginEditor.cpp
        src/PluginEditor.h
        src/effects/Fuzz.cpp
        src/effects/Fuzz.h
        src/dsp/Filter.cpp
        src/dsp/Filter.h)

target_compile_definitions(OpenGuitar_Fuzz
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

target_link_libraries(OpenGuitar_Fuzz
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Copy Fuzz VST3 to custom directory
add_custom_command(TARGET OpenGuitar_Fuzz_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND robocopy "$<TARGET_FILE_DIR:OpenGuitar_Fuzz_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar Fuzz.vst3" /E /IS /IT /R:0 /W:0 /NP /XJD /XJF || (exit 0)
    COMMENT "Copying Fuzz VST3 to openEff/vst/"
)

# Compressor Plugin
juce_add_plugin(OpenGuitar_Compressor
    COMPANY_NAME "OpenGuitar"
    PLUGIN_MANUFACTURER_CODE Ogtr
    PLUGIN_CODE Ogcp
    FORMATS VST3
    PRODUCT_NAME "OpenGuitar Compressor"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx" "Dynamics")

target_sources(OpenGuitar_Compressor
    PRIVATE
        src/CompressorProcessor.cpp
        src/CompressorProcessor.h
        src/CompressorEditor.cpp
        src/CompressorEditor.h
        src/effects/Compressor.cpp
        src/effects/Compressor.h)

target_compile_definitions(OpenGuitar_Compressor
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

target_link_libraries(OpenGuitar_Compressor
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Copy Compressor VST3 to custom directory
add_custom_command(TARGET OpenGuitar_Compressor_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND robocopy "$<TARGET_FILE_DIR:OpenGuitar_Compressor_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar Compressor.vst3" /E /IS /IT /R:0 /W:0 /NP /XJD /XJF || (exit 0)
    COMMENT "Copying Compressor VST3 to openEff/vst/"
)

# Reverb Plugin
juce_add_plugin(OpenGuitar_Reverb
    COMPANY_NAME "OpenGuitar"
    PLUGIN_MANUFACTURER_CODE Ogtr
    PLUGIN_CODE Ogrv
    FORMATS VST3
    PRODUCT_NAME "OpenGuitar Reverb"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx" "Reverb")

target_sources(OpenGuitar_Reverb
    PRIVATE
        src/ReverbProcessor.cpp
        src/ReverbProcessor.h
        src/ReverbEditor.cpp
        src/ReverbEditor.h
        src/effects/Reverb.cpp
        src/effects/Reverb.h)

target_compile_definitions(OpenGuitar_Reverb
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

target_link_libraries(OpenGuitar_Reverb
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Copy Reverb VST3 to custom directory
add_custom_command(TARGET OpenGuitar_Reverb_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND robocopy "$<TARGET_FILE_DIR:OpenGuitar_Reverb_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar Reverb.vst3" /E /IS /IT /R:0 /W:0 /NP /XJD /XJF || (exit 0)
    COMMENT "Copying Reverb VST3 to openEff/vst/"
)
