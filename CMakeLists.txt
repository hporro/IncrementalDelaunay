cmake_minimum_required(VERSION 3.1)

project(Delaunay_visual VERSION 0.1 LANGUAGES CXX C)
set (CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g3 -ggdb3 -fno-inline")

# Download submodules
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

# GLFW: window manager for OpenGL context
add_subdirectory(extern/glfw-3.3.2)

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

# Delaunay Data Structure manipulation and drawing files
set(DELAUNAY_VISUAL_SOURCES src/draw_triangulation.h src/delaunay.h src/delaunay.cpp src/point_generator.h src/point_generator.cpp src/constants.h)
add_library(delaunay_visual ${DELAUNAY_VISUAL_SOURCES})
target_link_libraries(delaunay_visual PUBLIC glm gl3w)

# DelaunayGUI: GUI for the application
set(DELAUNAY_GUI_SOURCES src/GUI/gui.cpp)
add_library(DelaunayGUI ${DELAUNAY_GUI_SOURCES})
target_link_libraries(DelaunayGUI PRIVATE imgui)
target_link_libraries(DelaunayGUI PUBLIC glfw gl3w delaunay_visual)

# tests
add_executable(tests test/tests.cpp)
target_link_libraries(tests PRIVATE glm delaunay_visual glfw gl3w)

# Executable to try out stuff, it's not important
add_executable(playground playground.cpp src/delaunay.h src/delaunay.cpp src/point_generator.h src/point_generator.cpp src/constants.h)
target_link_libraries(playground PRIVATE glm)

# Copy shaders to executable file folder
file(GLOB SHADERS
  "src/shaders/*.vert"
  "src/shaders/*.frag"
)
foreach(SHADER ${SHADERS})
    file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
endforeach(SHADER)

# Experiments executable
add_executable(Experiments Experiments/exps.cpp)
target_link_libraries(Experiments PRIVATE glm delaunay_visual glfw gl3w)


message(status ${CMAKE_DL_LIBS})

# Main app: All together
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})

# Main app2: All together, in a kinetic setting
add_executable(simulation src/simulation.cpp)
target_link_libraries(simulation PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})
target_include_directories(simulation PRIVATE src)
