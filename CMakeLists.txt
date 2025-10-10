<<<<<<< HEAD
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
=======
cmake_minimum_required(VERSION 3.24)
project(WebSocketNetworking)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

option(ENABLE_NCURSES_CLIENT "Build the NCurses client" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(tools)

>>>>>>> origin/simplified-networking-chat-room
