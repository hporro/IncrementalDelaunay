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

add_library(preds extern/pred3d.cpp)
# Gonna use exactinit() and orient2d(...)

add_library(delaunay_geometry src/delaunay.h src/delaunay.cpp)
target_link_libraries(delaunay_geometry PRIVATE preds glm)
target_include_directories(delaunay_geometry PRIVATE extern)

# imgui: GUI library
include_directories(extern/imgui extern/imgui/examples)
set(IMGUI_SOURCES extern/imgui/examples/imgui_impl_glfw.cpp extern/imgui/examples/imgui_impl_opengl3.cpp extern/imgui/imgui.cpp extern/imgui/imgui_demo.cpp extern/imgui/imgui_draw.cpp extern/imgui/imgui_widgets.cpp)
add_library(imgui ${IMGUI_SOURCES})
target_link_libraries(imgui PUBLIC glfw gl3w)


# Delaunay Data Structure manipulation and drawing files
set(DELAUNAY_VISUAL_SOURCES src/draw_triangulation.h src/point_generator.h src/point_generator.cpp src/constants.h)
add_library(delaunay_visual ${DELAUNAY_VISUAL_SOURCES})
target_link_libraries(delaunay_visual PUBLIC glm gl3w preds delaunay_geometry)
target_include_directories(delaunay_visual PRIVATE extern)

# DelaunayGUI: GUI for the application
set(DELAUNAY_GUI_SOURCES src/GUI/gui.cpp)
add_library(DelaunayGUI ${DELAUNAY_GUI_SOURCES})
target_link_libraries(DelaunayGUI PRIVATE imgui)
target_link_libraries(DelaunayGUI PUBLIC glfw gl3w delaunay_visual delaunay_geometry)
target_include_directories(DelaunayGUI PRIVATE extern)

# tests
add_executable(tests test/tests.cpp)
target_link_libraries(tests PRIVATE glm delaunay_visual glfw gl3w)
target_include_directories(tests PRIVATE extern)

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
target_include_directories(Experiments PRIVATE extern)

message(status ${CMAKE_DL_LIBS})

# Main app: All together
add_executable(main examples/main.cpp)
target_link_libraries(main PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})
target_include_directories(main PRIVATE src)
target_include_directories(main PRIVATE extern)

# Main app2: All together, in a kinetic setting
add_executable(simulation examples/simulation.cpp)
target_link_libraries(simulation PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})
target_include_directories(simulation PRIVATE src)
target_include_directories(simulation PRIVATE extern)

# Main app3: Playground to test the deleteVertex and ReaddVertex methods 
add_executable(deleteVertex examples/deleteVertex.cpp)
target_link_libraries(deleteVertex PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})
target_include_directories(deleteVertex PRIVATE src)
target_include_directories(deleteVertex PRIVATE extern)

# Main app3: Playground to test the deleteVertex and ReaddVertex methods 
add_executable(deleteVertexsmol examples/littleDeleteVertex.cpp)
target_link_libraries(deleteVertexsmol PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})
target_include_directories(deleteVertexsmol PRIVATE src)
target_include_directories(deleteVertexsmol PRIVATE extern)

# Main app4: Boids simulation 
add_executable(boids examples/boids.cpp)
target_link_libraries(boids PRIVATE delaunay_visual glfw gl3w DelaunayGUI ${CMAKE_DL_LIBS})
target_include_directories(boids PRIVATE src)
target_include_directories(boids PRIVATE extern)