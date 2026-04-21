cmake_minimum_required(VERSION 3.14)

project(imgui)
add_library(imgui STATIC)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

file(GLOB_RECURSE IMGUI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

target_sources(imgui PRIVATE ${IMGUI_SRC})