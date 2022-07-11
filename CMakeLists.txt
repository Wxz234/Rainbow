cmake_minimum_required(VERSION 3.20)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(ThirdParty/D3D12MemoryAllocator)
set_property(TARGET D3D12MemoryAllocator PROPERTY FOLDER "ThirdParty")

add_library(imgui STATIC 
	ThirdParty/imgui/backends/imgui_impl_win32.cpp
	ThirdParty/imgui/backends/imgui_impl_win32.h
	ThirdParty/imgui/backends/imgui_impl_dx12.cpp
	ThirdParty/imgui/backends/imgui_impl_dx12.h
	ThirdParty/imgui/imconfig.h
	ThirdParty/imgui/imgui.cpp
	ThirdParty/imgui/imgui.h
	ThirdParty/imgui/imgui_demo.cpp
	ThirdParty/imgui/imgui_draw.cpp
	ThirdParty/imgui/imgui_internal.h
	ThirdParty/imgui/imgui_tables.cpp
	ThirdParty/imgui/imgui_widgets.cpp
	ThirdParty/imgui/imstb_rectpack.h
	ThirdParty/imgui/imstb_textedit.h
	ThirdParty/imgui/imstb_truetype.h
)
target_include_directories(imgui PRIVATE "ThirdParty/imgui/")
set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")

add_library(RainbowCore STATIC Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE)

set_source_files_properties(${CMAKE_SOURCE_DIR}/Shader/SceneMeshShader.hlsl PROPERTIES VS_SHADER_VARIABLE_NAME "SceneMeshShader" VS_SHADER_OUTPUT_HEADER_FILE "${CMAKE_SOURCE_DIR}/Shader/SceneMeshShader.h" VS_SHADER_TYPE Mesh VS_SHADER_MODEL 6.5 VS_SHADER_ENTRYPOINT main)
add_library(RainbowShader STATIC
	Shader/SceneMeshShader.hlsl
	Shader/Shader.cpp
)

add_library(RainbowRuntime STATIC
	Runtime/Scene.cpp 
	Runtime/GameObject.cpp 
	Runtime/LoadGLTF.cpp
	Runtime/CameraObject.cpp
)
target_compile_definitions(RainbowRuntime PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowRuntime PRIVATE RainbowShader)

target_link_libraries(RainbowShader PRIVATE RainbowCore)
target_compile_definitions(RainbowShader PRIVATE UNICODE _UNICODE)
target_include_directories(RainbowShader PRIVATE "${CMAKE_SOURCE_DIR}/Include")


add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowEditor PRIVATE RainbowCore RainbowRuntime)
target_link_libraries(RainbowEditor PRIVATE D3D12MemoryAllocator imgui)
target_link_libraries(RainbowEditor PRIVATE d3d12.lib dxgi.lib dxguid.lib d3dcompiler.lib)
