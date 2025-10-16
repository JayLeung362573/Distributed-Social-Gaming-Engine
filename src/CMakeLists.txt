cmake_minimum_required(VERSION 3.28.2)
add_library(core_lib
  lib/add.cpp
  GameClient.cpp
  GameServer.cpp
  Networking.cpp
)

target_include_directories(core_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE core_lib)
target_compile_features(main PRIVATE cxx_std_23)
