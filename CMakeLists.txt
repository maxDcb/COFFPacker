cmake_minimum_required(VERSION 3.5)
project(CoffPacker VERSION 1.0.0 LANGUAGES CXX)

set(DEFAULT_BUILD_TYPE "Release")
#set(CMAKE_C_STANDARD 17)

if(WIN32)
	set(CMAKE_C_FLAGS_RELEASE "/MT")
	set(CMAKE_CXX_FLAGS_RELEASE "/MT")
endif()

include_directories(struct)

set(SOURCE_FILES
CoffPacker.cpp
struct/struct_endian.c
struct/struct.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ./)

add_executable(testsCoffPacker testsCoffPacker.cpp)
target_link_libraries(testsCoffPacker CoffPacker)
