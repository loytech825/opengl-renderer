cmake_minimum_required(VERSION 3.14)

project(stb_image)
add_library(stb_image STATIC)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(stb_image PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.cpp)