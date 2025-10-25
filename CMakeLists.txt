cmake_minimum_required(VERSION 3.28.2)

project(main)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

# This part automatically initializes and updates git submodules (so we don't have to do run
# it manually when cloning/pulling from the repo)
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  message(STATUS "Checking for submodules...")

  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/spdlog/CMakeLists.txt")
    message(STATUS "Submodules not found, running 'git submodule update --init --recursive'")
    execute_process(
      COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      RESULT_VARIABLE GIT_SUBMOD_RESULT
    )

    if(NOT GIT_SUBMOD_RESULT EQUAL 0)
      message(FATAL_ERROR
        "Git submodule update failed. "
        "Please run 'git submodule update --init --recursive' manually.")
    endif()
  endif()
endif()
## End of git submodule initialization

if(NOT TARGET spdlog AND NOT TARGET "spdlog::spdlog")
  add_subdirectory(external/spdlog)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/src/CMakeLists.txt")
  add_subdirectory(src)
endif()

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
