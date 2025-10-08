cmake_minimum_required(VERSION 3.28.2)

project(main)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

add_executable(main)
target_sources(main
  PRIVATE
    main.cpp
    GameClient.cpp
    GameServer.cpp
    Networking.cpp
)

target_compile_features(main
  PUBLIC
    cxx_std_23
)
