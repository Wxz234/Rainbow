cmake_minimum_required(VERSION 3.24)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)

add_library(RainbowCore SHARED Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE RAINBOW_LIB)

add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowEditor PRIVATE RainbowCore)
