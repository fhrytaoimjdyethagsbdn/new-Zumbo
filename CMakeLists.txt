cmake_minimum_required(VERSION 3.22)
project(PSY_ZUMBO VERSION 0.1.0)

include(FetchContent)

set(JUCE_VERSION 8.0.10 CACHE STRING "JUCE version")
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG ${JUCE_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(PSY_ZUMBO
    COMPANY_NAME "PSY ZUMBO Audio"
    BUNDLE_ID "com.psyzumbo.audio"
    PLUGIN_MANUFACTURER_CODE PZUM
    PLUGIN_CODE PZ01
    FORMATS VST3
    PRODUCT_NAME "PSY - ZUMBO"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(PSY_ZUMBO PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(PSY_ZUMBO PRIVATE cxx_std_17)

target_compile_definitions(PSY_ZUMBO PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(PSY_ZUMBO PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

if (MSVC)
    target_compile_options(PSY_ZUMBO PRIVATE /O2 /Ob2 /Oi /Ot)
endif()
