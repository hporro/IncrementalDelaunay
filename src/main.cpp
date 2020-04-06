#include <iostream>

#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "GUI/gui.h"
#include "point_generator.h"

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
    Vec2 p1 = Vec2(10.0,10.0);
    Vec2 p2 = Vec2(20.0,10.0);
    Vec2 p3 = Vec2(20.0,20.0);
    std::vector<Vec2> points = POINT_GENERATOR::gen_points_triangle(1,p1,p2,p3);
    POINT_GENERATOR::print_points(points);

    //gen triangulation
    Triangle t(p1,p2,p3);
    Triangulation tri(points,&t);
    tri.print();

    // Main loop
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(gstate->clear_color.x, gstate->clear_color.y, gstate->clear_color.z, gstate->clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        //dgui->draw();
        //dgui->render();
        glfwSwapBuffers(window);
    }

    delete dgui;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}