cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME kuso_test)
set(CMAKE_BUILD_TYPE Debug)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

set(SRCS_DIR ./../src)
set(INCLUDE_DIR ./../include)
set(DEPS_DIR ./../deps)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
  ${PROJECT_NAME}
  Testing.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${SRCS_DIR} srcs)
add_subdirectory(src)


target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${INCLUDE_DIR}
  ${DEPS_DIR}
)

include(FetchContent)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.1.1
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(${DEPS_DIR}/pirate pirate)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  GTest::gtest_main
  GTest::gmock_main
  pirate
  fmt::fmt
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})