cmake_minimum_required(VERSION 3.11)

# Include FetchContent module
include(FetchContent)

# Set FETCHCONTENT_UPDATES_DISCONNECTED to TRUE to avoid updating dependencies
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Declare ftxui dependency
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)

# Make ftxui available
FetchContent_MakeAvailable(ftxui)

# Project definition
project(visi LANGUAGES CXX VERSION 1.0.0)

# Add executable
add_executable(visi src/main.cpp)

# Link libraries
target_link_libraries(visi
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE pulse
  PRIVATE pulse-simple
  PRIVATE sdbus-c++
)

# Find PulseAudio libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED libpulse libpulse-simple)

# Include directories for PulseAudio
target_include_directories(visi PRIVATE ${PULSE_INCLUDE_DIRS})

# Link PulseAudio libraries
target_link_libraries(visi PRIVATE ${PULSE_LIBRARIES})
