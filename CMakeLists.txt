cmake_minimum_required(VERSION 3.24)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)


add_library(dxcompiler SHARED IMPORTED)
set_property(TARGET dxcompiler PROPERTY
             IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/ThirdParty/DirectXShaderCompiler/dxcompiler.dll")
set_property(TARGET dxcompiler PROPERTY
             IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/ThirdParty/DirectXShaderCompiler/dxcompiler.lib")

add_library(RainbowCore SHARED Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE RAINBOW_LIB)
#target_link_libraries(RainbowCore PRIVATE dxc)
target_link_libraries(RainbowCore PUBLIC dxcompiler)

add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
#target_link_libraries(RainbowEditor dxc)
target_link_libraries(RainbowEditor PRIVATE RainbowCore)
#target_link_libraries(RainbowEditor dxc)
