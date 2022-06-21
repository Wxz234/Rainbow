cmake_minimum_required(VERSION 3.20)

project(Rainbow)
set(CMAKE_CXX_STANDARD 20)

add_library(RainbowCore STATIC Core/Core.cpp)
target_include_directories(RainbowCore PRIVATE "Include")