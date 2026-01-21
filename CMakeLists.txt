cmake_minimum_required(VERSION 3.15)

# Read current version from version.txt
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

# Parse version (format: MAJOR.MINOR.PATCH)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" VERSION_MATCH "${VERSION_STRING}")
if(VERSION_MATCH)
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    set(VERSION_MINOR ${CMAKE_MATCH_2})
    set(VERSION_PATCH ${CMAKE_MATCH_3})
else()
    # Fallback if version.txt is invalid
    set(VERSION_MAJOR 1)
    set(VERSION_MINOR 0)
    set(VERSION_PATCH 0)
endif()

project(OpenGuitar VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

message(STATUS "Building OpenGuitar version ${PROJECT_VERSION}")

# Increment version for next build
math(EXPR NEXT_PATCH "${VERSION_PATCH} + 1")
set(NEXT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${NEXT_PATCH}")
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" "${NEXT_VERSION}")
message(STATUS "Next build will be version ${NEXT_VERSION}")

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

# Copy Fuzz VST3 to custom directory (cross-platform)
add_custom_command(TARGET OpenGuitar_Fuzz_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:OpenGuitar_Fuzz_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar Fuzz.vst3"
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

# Copy Compressor VST3 to custom directory (cross-platform)
add_custom_command(TARGET OpenGuitar_Compressor_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:OpenGuitar_Compressor_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar Compressor.vst3"
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

# Copy Reverb VST3 to custom directory (cross-platform)
add_custom_command(TARGET OpenGuitar_Reverb_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:OpenGuitar_Reverb_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar Reverb.vst3"
    COMMENT "Copying Reverb VST3 to openEff/vst/"
)

# Pedal Board Plugin
juce_add_plugin(OpenGuitar_PedalBoard
    COMPANY_NAME "OpenGuitar"
    PLUGIN_MANUFACTURER_CODE Ogtr
    PLUGIN_CODE Ogpb
    FORMATS VST3
    PRODUCT_NAME "OpenGuitar Pedal Board"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx" "Tools")

target_sources(OpenGuitar_PedalBoard
    PRIVATE
        src/pedalboard/PedalBoardProcessor.cpp
        src/pedalboard/PedalBoardProcessor.h
        src/pedalboard/PedalBoardEditor.cpp
        src/pedalboard/PedalBoardEditor.h
        src/pedalboard/EffectChain.cpp
        src/pedalboard/EffectChain.h
        src/pedalboard/EffectFactory.cpp
        src/pedalboard/EffectFactory.h
        src/pedalboard/ui/PedalComponent.cpp
        src/pedalboard/ui/PedalComponent.h
        src/pedalboard/ui/PedalLookAndFeel.cpp
        src/pedalboard/ui/PedalLookAndFeel.h
        src/effects/EffectBase.cpp
        src/effects/EffectBase.h
        src/effects/Fuzz.cpp
        src/effects/Fuzz.h
        src/effects/Compressor.cpp
        src/effects/Compressor.h
        src/effects/Reverb.cpp
        src/effects/Reverb.h
        src/effects/Chorus.cpp
        src/effects/Chorus.h
        src/effects/Orange.cpp
        src/effects/Orange.h
        src/effects/BigMuff.cpp
        src/effects/BigMuff.h
        src/effects/Tuner.cpp
        src/effects/Tuner.h
        src/dsp/Filter.cpp
        src/dsp/Filter.h)

target_compile_definitions(OpenGuitar_PedalBoard
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

target_link_libraries(OpenGuitar_PedalBoard
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Copy PedalBoard VST3 to custom directory
add_custom_command(TARGET OpenGuitar_PedalBoard_VST3 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/openEff/vst"
    COMMAND robocopy "$<TARGET_FILE_DIR:OpenGuitar_PedalBoard_VST3>/../.." "${CMAKE_SOURCE_DIR}/openEff/vst/OpenGuitar PedalBoard.vst3" /E /IS /IT /R:0 /W:0 /NP /XJD /XJF || (exit 0)
    COMMENT "Copying PedalBoard VST3 to openEff/vst/"
)
