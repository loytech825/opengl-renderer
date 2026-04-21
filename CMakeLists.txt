cmake_minimum_required(VERSION 3.20)

project(imgui_glfw_core)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW3)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb_image)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**" )
#file(GLOB_RECURSE IMGUI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/*.cpp")

add_executable(app)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)
target_sources(app PRIVATE ${SOURCES} ${IMGUI_SRC})
target_link_libraries(app PRIVATE glfw glad stb_image glm imgui)