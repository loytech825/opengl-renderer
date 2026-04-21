cmake_minimum_required(VERSION 3.14)

project(glad)
set(CMAKE_CXX_STANDARD 17)

add_library(glad STATIC)
target_sources(glad PRIVATE src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)