cmake_minimum_required(VERSION 3.1)

project(Delaunay_visual VERSION 0.1 LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 11)

# GLFW: window manager for OpenGL context
find_package(glfw3 3.3 REQUIRED)

# GLM: Math library
add_subdirectory(extern/glm)

# gl3w: OpenGL context loader
include_directories(extern/gl3w/include)
set(GL3W_SOURCES extern/gl3w/src/gl3w.c)
add_library(gl3w ${GL3W_SOURCES})

# imgui: GUI library
include_directories(extern/imgui extern/imgui/examples)
set(IMGUI_SOURCES extern/imgui/examples/imgui_impl_glfw.cpp extern/imgui/examples/imgui_impl_opengl3.cpp extern/imgui/imgui.cpp extern/imgui/imgui_demo.cpp extern/imgui/imgui_draw.cpp extern/imgui/imgui_widgets.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_link_libraries(imgui PUBLIC glfw gl3w)

# DelaunayGUI: GUI for the application
set(DELAUNAY_GUI_SOURCES src/GUI/gui.cpp)
add_library(DelaunayGUI ${DELAUNAY_GUI_SOURCES})
target_link_libraries(DelaunayGUI PRIVATE imgui)
target_link_libraries(DelaunayGUI PUBLIC glfw gl3w)

# Delaunay Data Structure manipulation and drawing files
set(DELAUNAY_VISUAL_SOURCES src/draw_triangulation.h src/delaunay.h src/delaunay.cpp src/point_generator.h src/point_generator.cpp src/constants.h)
add_library(delaunay_visual ${DELAUNAY_VISUAL_SOURCES})
target_link_libraries(delaunay_visual PUBLIC glm gl3w)

# Main app: All together
add_executable(main src/main.cpp ../extern/imgui/imgui.h)
target_link_libraries(main PRIVATE delaunay_visual glfw3 gl3w DelaunayGUI)

# Executable to try out stuff, it's not important
add_executable(playground playground.cpp src/delaunay.h src/delaunay.cpp src/point_generator.h src/point_generator.cpp src/constants.h)
target_link_libraries(playground PRIVATE glm)