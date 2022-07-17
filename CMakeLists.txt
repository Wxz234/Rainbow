cmake_minimum_required(VERSION 3.24)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)


add_library(dxc SHARED IMPORTED)
set_property(TARGET dxc PROPERTY IMPORTED_LOCATION C:/Users/42937/Documents/GitHub/Rainbow/ThirdParty/dxc/dxcompiler.dll)
set_property(TARGET dxc PROPERTY IMPORTED_IMPLIB C:/Users/42937/Documents/GitHub/Rainbow/ThirdParty/dxc/dxcompiler.lib)

add_library(RainbowCore SHARED Core/Renderer.cpp Core/Window.cpp)
target_compile_definitions(RainbowCore PRIVATE UNICODE _UNICODE RAINBOW_LIB)
target_link_libraries(RainbowCore PRIVATE dxc d3d12.lib dxgi.lib dxguid.lib d3dcompiler.lib)
add_custom_command(TARGET RainbowCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:RainbowCore> $<TARGET_FILE_DIR:RainbowCore>
	COMMAND_EXPAND_LISTS
)

add_custom_command(TARGET RainbowCore POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/ThirdParty/dxc/dxil.dll" $<TARGET_FILE_DIR:RainbowCore>
)

add_executable(RainbowDemo01_CreateWindow WIN32 Demo/CreateWindow/CreateWindow.cpp)
target_compile_definitions(RainbowDemo01_CreateWindow PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowDemo01_CreateWindow PRIVATE RainbowCore)

add_executable(RainbowDemo02_DrawTriangle WIN32 Demo/DrawTriangle/DrawTriangle.cpp)
target_compile_definitions(RainbowDemo02_DrawTriangle PRIVATE UNICODE _UNICODE)
target_link_libraries(RainbowDemo02_DrawTriangle PRIVATE RainbowCore)
set_target_properties(RainbowDemo02_DrawTriangle PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/Demo/DrawTriangle")
add_custom_command(TARGET RainbowDemo02_DrawTriangle PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/Demo/DrawTriangle/Shader" $<TARGET_FILE_DIR:RainbowDemo02_DrawTriangle>/Shader
)
