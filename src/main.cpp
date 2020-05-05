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

    int numP = 1000;

    Vec2 p10 = Vec2(-0.89,-0.89);
    Vec2 p11 = Vec2(0.89,-0.89);
    Vec2 p12 = Vec2(0.89,0.89);
    Vec2 p13 = Vec2(-0.89,0.89);

    std::vector<Vec2> points = POINT_GENERATOR::gen_points_square(numP,p10,p11,p12,p13);
    Triangulation *t = new Triangulation(points,points.size());
    TriangulationDrawer *td = new TriangulationDrawer(t);

    // Gui for the program
    GUIState* gstate = new GUIState();
    DelaunayGUI* dgui = new DelaunayGUI();
    dgui->state = gstate;
    dgui->init(window);
    dgui->state->t = t;
    dgui->state->numT = t->tcount;
    dgui->state->td = td;

    glEnable( GL_PROGRAM_POINT_SIZE );
    // Main loop
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if(dgui->state->newTriagulationNeeded){
            delete t;
            delete td;

            dgui->state->newTriagulationNeeded = false;

            points = POINT_GENERATOR::gen_points_square(gstate->futNumP,p10,p11,p12,p13);
            t = new Triangulation(points,points.size());
            td = new TriangulationDrawer(t);
            gstate->numP = t->vcount;
            gstate->numT = t->tcount;
        }
        glPointSize(dgui->state->PointSize);
        td->setColor(dgui->state->triangulation_color);
        td->draw();
        if(dgui->state->ShowPoints)
            td->draw_points();
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