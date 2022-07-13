cmake_minimum_required(VERSION 3.20)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)

add_library(RainbowCore STATIC Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE)

set_source_files_properties(${CMAKE_SOURCE_DIR}/Shader/SceneMeshShader.hlsl PROPERTIES VS_SHADER_VARIABLE_NAME "SceneMeshShader" VS_SHADER_OUTPUT_HEADER_FILE "${CMAKE_SOURCE_DIR}/Shader/SceneMeshShader.h" VS_SHADER_TYPE Mesh VS_SHADER_MODEL 6.5 VS_SHADER_ENTRYPOINT main)
add_library(RainbowShader STATIC
	Shader/SceneMeshShader.hlsl
	Shader/Shader.cpp
)


target_link_libraries(RainbowShader PRIVATE RainbowCore)
target_compile_definitions(RainbowShader PRIVATE UNICODE _UNICODE)
target_include_directories(RainbowShader PRIVATE "${CMAKE_SOURCE_DIR}/Include")


add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowEditor PRIVATE RainbowCore)
target_link_libraries(RainbowEditor PRIVATE d3d12.lib dxgi.lib dxguid.lib d3dcompiler.lib)
