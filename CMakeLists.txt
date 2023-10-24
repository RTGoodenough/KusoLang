cmake_minimum_required(VERSION 3.17)
set(PROJECT_NAME KusoLang)
project(${PROJECT_NAME} CXX)

# Debug Mode option
option(DEBUG_MODE "Debug Mode" OFF)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the source, include, dependencies and generated directories
set(SRC_DIR ./src)
set(INC_DIR ./include)
set(GEN_DIR ./grammar/generated)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp)

# Set the build type
if(DEBUG_MODE)
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set the include directory
target_include_directories(
  ${PROJECT_NAME} 
  PUBLIC
  ${INC_DIR}
)

# Set the source directory
add_subdirectory(${SRC_DIR})
add_subdirectory(${GEN_DIR})

# Setting up libraries

message(STATUS "Setting Up ANTLR")
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/antlr_setup )
include( ExternalAntlr4Cpp )
link_directories( ${ANTLR4CPP_LIBS} )
message(STATUS "Done Setting Up ANTLR")

include(FetchContent)
message(STATUS "Getting spdlog")
FetchContent_Declare(spdlog URL https://github.com/gabime/spdlog/archive/v1.11.0.tar.gz)
FetchContent_MakeAvailable(spdlog)
message(STATUS "Done Getting spdlog")

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  spdlog::spdlog
  antlr4_static
)

include_directories(
  SYSTEM
  ${GEN_DIR}
  ${ANTLR4CPP_INCLUDE_DIRS}
)