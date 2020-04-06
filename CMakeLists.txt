cmake_minimum_required(VERSION 3.1)

project(Delaunay_visual VERSION 0.1 LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 11)

# GLFW: window manager for OpenGL context
find_package(glfw3 3.3 REQUIRED)

# Homework implementation files
set(DELAUNAY_VISUAL_SOURCES src/delaunay.h src/delaunay.cpp)
add_library(delaunay_visual ${DELAUNAY_VISUAL_SOURCES})

# gl3w: OpenGL context loader
include_directories(external/gl3w/include)
set(GL3W_SOURCES external/gl3w/src/gl3w.c)
add_library(gl3w ${GL3W_SOURCES})

# imgui: GUI library
include_directories(external/imgui external/imgui/examples)
set(IMGUI_SOURCES external/imgui/examples/imgui_impl_glfw.cpp external/imgui/examples/imgui_impl_opengl3.cpp external/imgui/imgui.cpp external/imgui/imgui_demo.cpp external/imgui/imgui_draw.cpp external/imgui/imgui_widgets.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_link_libraries(imgui PUBLIC glfw gl3w)

# main app
add_executable(main src/main.cpp ../external/imgui/imgui.h)
target_link_libraries(main PRIVATE delaunay_visual glfw3 gl3w imgui)
add_link_options(main -lGLEW)