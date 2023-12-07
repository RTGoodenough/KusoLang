cmake_minimum_required(VERSION 3.24)

option(DEBUG "Debug mode" OFF)

if(DEBUG)
  message("Debug mode")
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null")
else()
  message("Release mode")
  set(CMAKE_BUILD_TYPE Release)
endif()

project(Morasu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(deps_dir ${CMAKE_CURRENT_SOURCE_DIR}/deps)

add_executable(
  ${PROJECT_NAME}
  ${src_dir}/main.cpp
)

add_subdirectory(${src_dir})


target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${include_dir}
)
  
include_directories(
  SYSTEM
  ${deps_dir}
)
    
add_subdirectory(${deps_dir}/pirate pirate)
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  pirate
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Werror
)
