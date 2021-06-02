cmake_minimum_required (VERSION 3.11)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG 2445dc03d33cfcfcb1882c24b4c662ae6f675fd5
  # Specify a GIT TAG here.
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

project(fungi
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(fungi
  src/fungi/main.cpp
  src/fungi/panel/panel.hpp
  src/fungi/panel/sound/sound.cpp
  src/fungi/panel/placeholder/placeholder.cpp
)
target_include_directories(fungi PRIVATE src)

find_package(ALSA REQUIRED)

target_link_libraries(fungi
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
  PRIVATE ALSA::ALSA
)

# C++17 is used. We requires fold expressions at least.
set_target_properties(fungi PROPERTIES CXX_STANDARD 17)

install(TARGETS fungi RUNTIME DESTINATION "bin")
