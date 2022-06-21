cmake_minimum_required(VERSION 3.20)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)

add_library(RainbowCore STATIC Core/Core.cpp)
target_include_directories(RainbowCore PRIVATE "Include")

add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_include_directories(RainbowEditor PRIVATE "Include")
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)