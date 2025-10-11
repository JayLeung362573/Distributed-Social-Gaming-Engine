cmake_minimum_required(VERSION 3.28.2)

project(main)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti -fsanitize=address,undefined") # Enable Address sanitizer and enforce static casts

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()


# For Remote dependencies
include(cmake/CPM.cmake)

CPMAddPackage(
  NAME web-socket-networking
  GITHUB_REPOSITORY nsumner/web-socket-networking
  GIT_TAG master
)

option(ENABLE_NCURSES_CLIENT "Build the NCurses client" ON)
option(ENABLE_FLUTTER_CLIENT "Build the Flutter client" OFF) # Manually turn build off
option(ENABLE_FTXUI_CLIENT   "Build the FTXUI client" OFF) # Manually turn build off

add_executable(main)
target_sources(main
  PRIVATE
    main.cpp
    GameClient.cpp
    GameServer.cpp
    Networking.cpp
    WebsocketNetworking.cpp
)

target_link_libraries(main
  PRIVATE
    networking
)

target_compile_features(main
  PUBLIC
    cxx_std_23
)
