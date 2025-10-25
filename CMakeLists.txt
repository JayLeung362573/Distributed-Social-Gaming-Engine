cmake_minimum_required(VERSION 3.28.2)

project(main)

# Default to Debug build if not specified
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Enable sanitizers only for Debug builds, but retain static casts enforcement
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Running Debug build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti -fsanitize=address,undefined") # Enable Address sanitizer and enforce static casts
else()
    message(STATUS "Running Production build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

# For Remote dependencies
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME web-socket-networking
  GITHUB_REPOSITORY nsumner/web-socket-networking
  GIT_TAG master
  OPTIONS
    "ENABLE_FLUTTER_CLIENT OFF"
    "ENABLE_FTXUI_CLIENT OFF"
    "ENABLE_NCURSES_CLIENT ON"
)

add_executable(main)
target_sources(main
  PRIVATE
    src/main.cpp
    src/GameClient.cpp
    src/GameServer.cpp
    src/Networking.cpp
    src/WebsocketNetworking.cpp
    src/MessageTranslator.cpp
)

target_link_libraries(main
  PRIVATE
    networking
)

target_compile_features(main
  PUBLIC
    cxx_std_23
)
