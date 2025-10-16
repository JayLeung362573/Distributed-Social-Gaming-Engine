add_library(core_lib
  lib/add.cpp
)

target_include_directories(core_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_executable(sg_server main.cpp)
target_link_libraries(sg_server PRIVATE core_lib)
target_compile_features(sg_server PRIVATE cxx_std_23)
