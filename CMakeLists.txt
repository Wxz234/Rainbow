cmake_minimum_required(VERSION 3.24)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)


add_library(dxc SHARED IMPORTED)
set_property(TARGET dxc PROPERTY IMPORTED_LOCATION C:/Users/42937/Documents/GitHub/Rainbow/ThirdParty/dxc/dxcompiler.dll)
set_property(TARGET dxc PROPERTY IMPORTED_IMPLIB C:/Users/42937/Documents/GitHub/Rainbow/ThirdParty/dxc/dxcompiler.lib)

add_library(RainbowCore SHARED Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE RAINBOW_LIB)
target_link_libraries(RainbowCore PRIVATE dxc d3d12.lib dxgi.lib dxguid.lib d3dcompiler.lib)

add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowEditor PRIVATE RainbowCore dxguid.lib)
