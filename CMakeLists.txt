cmake_minimum_required(VERSION 3.20)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)

add_library(RainbowCore STATIC Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE)

add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowEditor PRIVATE RainbowCore)
target_link_libraries(RainbowEditor PRIVATE d3d12.lib dxgi.lib dxguid.lib)
