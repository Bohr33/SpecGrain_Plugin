cmake_minimum_required(VERSION 3.22)

project(PVExperiments VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(JUCE)

juce_add_plugin(PVExperiments
    VERSION "2.0.0"
    COMPANY_NAME "BWPlugins"

    BUNDLE_ID "com.BWPlugins.PVExperiments"

    PLUGIN_MANUFACTURER_CODE Bdub
    PLUGIN_CODE Vyge

    FORMATS AU VST3

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    PRODUCT_NAME "PVExperiments"
    AU_MAIN_TYPE "kAudioUnitType_Effect"

    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(PVExperiments)

# ============================================================
# Source Files
# ============================================================

target_sources(PVExperiments PRIVATE

    # Plugin Core
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h

    # PV Engine
    Source/DSP/PhaseVocoder.cpp
    Source/DSP/PhaseVocoder.h
    Source/DSP/PhaseVocoderEngine.cpp
    Source/DSP/PhaseVocoderEngine.h
    Source/DSP/SpectralProcessors.cpp
    Source/DSP/SpectralProcessors.h
    Source/DSP/Fsig.h
    Source/DSP/Window.h
    Source/DSP/Window.cpp

    # UI
    Source/GUI/Graphics.cpp
    Source/GUI/Graphics.h
    Source/GUI/GUIComponents.cpp
    Source/GUI/GUIComponents.h
    
)

target_include_directories(PVExperiments PRIVATE
    Source
    Source/DSP
    Source/GUI
)

# ============================================================
# Compile Definitions
# ============================================================

target_compile_definitions(PVExperiments PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# ============================================================
# Module Linking
# ============================================================

target_link_libraries(PVExperiments PRIVATE
    
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_core
    juce::juce_events
    juce::juce_graphics
    juce::juce_audio_basics

)


# ============================================================
# juce_add_binary_data to embed font
# ============================================================

# Binary Data - Font
juce_add_binary_data(PVExperiments_Font
    SOURCES
        Rescources/ZenMaruGothic-Light.ttf
)

# Link the generated binary data target to your plugin executable
target_link_libraries(PVExperiments PRIVATE
    PVExperiments_Font
    juce::juce_dsp
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_core
    juce::juce_events
    juce::juce_graphics
    juce::juce_audio_basics
)


