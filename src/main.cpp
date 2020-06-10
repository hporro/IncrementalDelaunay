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

GUIState* gstate = new GUIState();

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    gstate->zoom += yoffset==1?0.1:-0.1;
    if(gstate->zoom <= 0.1) gstate->zoom = 0.1;
}


double press_pos[2];
double release_pos[2];

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        glfwGetCursorPos(window, press_pos, press_pos+1);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        glfwGetCursorPos(window, release_pos, release_pos+1);
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        gstate->offset[0] -= ((float)(press_pos[0]-release_pos[0]))/width;
        gstate->offset[1] += ((float)(press_pos[1]-release_pos[1]))/height;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float off = 0.2;
    if (key == GLFW_KEY_W && action == GLFW_PRESS){
        gstate->offset[1] -= off;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS){
        gstate->offset[0] += off;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS){
        gstate->offset[1] += off;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS){
        gstate->offset[0] -= off;
    }
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
    GLFWwindow* window = glfwCreateWindow(700, 700, "Incremental Delaunay Triangulation", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;
    if (err)
    {
        std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
        return 1;
    }

    int numP = 1000;
    int a = 3;
    Vec2 p10 = Vec2(-0.8,-0.8);
    Vec2 p11 = Vec2(0.8,-0.8);
    Vec2 p12 = Vec2(0.8,0.8);
    Vec2 p13 = Vec2(-0.8,0.8);

    std::vector<Vec2> points = POINT_GENERATOR::gen_points_square(numP,p10,p11,p12,p13);
    Triangulation *t = new Triangulation(points,points.size(),true);
    TriangulationDrawer *td = new TriangulationDrawer(t);

    // Gui for the program
    DelaunayGUI* dgui = new DelaunayGUI();
    dgui->state = gstate;
    dgui->init(window);
    dgui->state->t = t;
    dgui->state->numT = t->tcount;
    dgui->state->numP = t->vcount;
    dgui->state->futNumP = numP;
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

            if (dgui->state->genGrid) {
                points = POINT_GENERATOR::gen_points_grid((int)sqrt(gstate->futNumP), (int)sqrt(gstate->futNumP), p10, p11, p12, p13);
            }
            if (!dgui->state->genGrid) {
                points = POINT_GENERATOR::gen_points_square(gstate->futNumP, p10, p11, p12, p13);
            }
            t = new Triangulation(points,points.size(),true);
            td = new TriangulationDrawer(t);
            gstate->numP = t->vcount;
            gstate->numT = t->tcount;
        }
        glPointSize(dgui->state->PointSize);
        td->setColor(dgui->state->triangulation_color);
        td->draw(dgui->state->offset,dgui->state->zoom);
        if(dgui->state->ShowPoints)
            td->draw_points(dgui->state->offset,dgui->state->zoom);
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