#include <stdio.h>

#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "GUI/gui.h"

static void glfw_error_callback(int error, const char* description){
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // gui for the program
    GUIState* gstate = new GUIState();
    DelaunayGUI* dgui = new DelaunayGUI();
    dgui->state = gstate;
    dgui->init(window);

    // Main loop
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(gstate->clear_color.x, gstate->clear_color.y, gstate->clear_color.z, gstate->clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        dgui->draw();
        dgui->render();
        glfwSwapBuffers(window);
    }

    delete dgui;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}