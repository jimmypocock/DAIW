cmake_minimum_required(VERSION 3.22)
project(DAIW VERSION 0.1.0)

# Add JUCE
add_subdirectory(JUCE)

# Create the application
juce_add_gui_app(DAIW
    PRODUCT_NAME "DAIW"
    COMPANY_NAME "DAIW"
    BUNDLE_ID "com.daiw.app"
)

# Generate JuceHeader.h for convenience
juce_generate_juce_header(DAIW)

# Source files
target_sources(DAIW PRIVATE
    src/Main.cpp
    src/MainComponent.cpp
)

# Include directories
target_include_directories(DAIW PRIVATE src)

# Link JUCE modules
target_link_libraries(DAIW PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Compile definitions
target_compile_definitions(DAIW PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:DAIW,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:DAIW,JUCE_VERSION>"
)

# C++ standard
target_compile_features(DAIW PRIVATE cxx_std_17)

# Compiler warnings (strict in debug)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(DAIW PRIVATE -Wall -Wextra -Wpedantic)
endif()
