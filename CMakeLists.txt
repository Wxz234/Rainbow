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
)
target_include_directories(imgui PRIVATE "ThirdParty/imgui/")
set_property(TARGET imgui PROPERTY FOLDER "ThirdParty")

add_library(RainbowCore STATIC Core/Core.cpp Core/Renderer.cpp)
target_include_directories(RainbowCore PRIVATE "Include")
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowCore PRIVATE D3D12MemoryAllocator)

add_library(RainbowRuntime STATIC Runtime/Runtime.cpp)
target_include_directories(RainbowRuntime PRIVATE "Include")
target_compile_definitions(RainbowRuntime PRIVATE UNICODE _UNICODE)

add_executable(RainbowEditor WIN32 Editor/Editor.cpp)
target_include_directories(RainbowEditor PRIVATE "Include")
target_compile_definitions(RainbowEditor PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowEditor PRIVATE RainbowCore RainbowRuntime)
target_link_libraries(RainbowEditor PRIVATE d3d12 dxgi dxguid)