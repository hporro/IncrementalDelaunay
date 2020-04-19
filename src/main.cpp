#include <iostream>

#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "GUI/gui.h"
#include "point_generator.h"
#include "delaunay.h"
#include "draw_triangulation.h"

static void glfw_error_callback(int error, const char* description){
    std::cerr << "Glfw Error "<< error << ": " << description << std::endl;
}

int main(int argn, char** argv){
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Incremental Delaunay Triangulation", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;
    if (err)
    {
        std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
        return 1;
    }

    // gui for the program
    GUIState* gstate = new GUIState();
    DelaunayGUI* dgui = new DelaunayGUI();
    dgui->state = gstate;
    dgui->init(window);

    //gen points
    Vec2 p0 = Vec2(-0.9,-0.9);
    Vec2 p1 = Vec2(0.9,-0.9);
    Vec2 p2 = Vec2(0.0,0.9);
    std::vector<Vec2> points = POINT_GENERATOR::gen_points_triangle(10,p0,p1,p2);

    //gen triangulation
    Triangulation tri = Triangulation(points,points.size(),p0,p1,p2);

    std::cout << tri.incount << " " << tri.edgecount << " " << tri.oedgecount << std::endl;
    TriangulationDrawer td(&tri);
    td.setColor(dgui->state->triangulation_color);

    // Main loop
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        td.setColor(dgui->state->triangulation_color);
        td.draw();
        glfwGetCursorPos(window, &dgui->state->xpos, &dgui->state->ypos);
        dgui->draw();
        dgui->render();
        glfwSwapBuffers(window);
    }

    delete dgui;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}