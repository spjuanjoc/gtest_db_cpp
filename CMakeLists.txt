cmake_minimum_required(VERSION 3.15)
project(gtest_demo_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread ")

#set(GTEST_DIR googletest)
#add_subdirectory(${GTEST_DIR})
add_definitions(-DGTEST_LANGUAGE_CXX11)
enable_testing()
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#message("gtest dir: ${gtest_SOURCE_DIR}")

add_executable(gtest_demo_cpp main.cpp)

target_link_libraries(gtest_demo_cpp gtest gmock )