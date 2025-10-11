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

add_library(ws
  WebSocket.cpp
  WebSocket.h
)
target_link_libraries(ws PRIVATE networking)
target_include_directories(ws PUBLIC
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib
  ${CMAKE_SOURCE_DIR}/lib/networking/include
)

add_executable(ws_demo ws_main.cpp GameServer.cpp GameClient.cpp Networking.cpp)
target_link_libraries(ws_demo PRIVATE ws networking)
