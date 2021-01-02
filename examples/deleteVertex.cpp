#include <iostream>

#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "GUI/gui.h"
#include "point_generator.h"
#include "delaunay.h"
#include "draw_triangulation.h"
#include "simulation/shakerSimulation.h"

static void glfw_error_callback(int error, const char* description){
    std::cerr << "Glfw Error "<< error << ": " << description << std::endl;
}

GUIState* gstate = new GUIState();

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    gstate->zoom += yoffset==1?0.1:-0.1;
    if(gstate->zoom <= 0.1) gstate->zoom = 0.1;
}

double clicked_pos[2];
bool mouse_dragging = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        glfwGetCursorPos(window, clicked_pos, clicked_pos+1);
        mouse_dragging = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        mouse_dragging = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(mouse_dragging){
        int width, height;
        glfwGetWindowSize(window ,&width, &height);
        gstate->offset[0] -= (clicked_pos[0] - xpos)/width;
        gstate->offset[1] += (clicked_pos[1] - ypos)/height;
        clicked_pos[0] = xpos;
        clicked_pos[1] = ypos;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float off = 0.01;
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
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Incremental Delaunay Triangulation", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;
    if (err)
    {
        std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
        return 1;
    }

    int numP = 800;
    float maxVel = 100;

    float boundSize = 300;
    Vec2 p10 = Vec2{-boundSize,-boundSize};
    Vec2 p11 = Vec2{boundSize,-boundSize };
    Vec2 p12 = Vec2{boundSize,boundSize  };
    Vec2 p13 = Vec2{-boundSize,boundSize };

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


    t->whichTriangle();
    std::set<int> n = t->getNeighbours(0);
    // for(int i=0;i<t->vcount;i++){
    //     std::cout << t->vertices[i].tri_index << " " << t->triangles[t->vertices[i].tri_index].v[0] << " " << t->triangles[t->vertices[i].tri_index].v[1] << " " << t->triangles[t->vertices[i].tri_index].v[2] << std::endl;
    // }std::cout << std::endl;
    // for(int v: n){
    //     std::cout << v << " ";
    // }std::cout << std::endl;

    TriangulationSaker* ts = new TriangulationSaker(t,maxVel);

    double currentTime = glfwGetTime(), lastTime = glfwGetTime();
    double time_passed = 0;

    glEnable( GL_PROGRAM_POINT_SIZE );
    // Main loop
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double delta = currentTime - lastTime;
        time_passed+=delta;
        if(dgui->state->runningsimulation){
            ts->step(1.0/60.0);
            td->genBuffers();
        }
        if(dgui->state->has_to_change_vel){
            delete ts;
            ts = new TriangulationSaker(t,dgui->state->maxVel);
            ts->initRandomVel();
        }
        lastTime = currentTime;
        currentTime = glfwGetTime();
        dgui->state->frame_count++;
        if(time_passed>=1.0){
            dgui->state->fps = dgui->state->frame_count;
            dgui->state->frame_count=0;
            time_passed=0;
        }

        if(dgui->state->newTriagulationNeeded){
            dgui->state->newTriagulationNeeded = false;

            if (dgui->state->genGrid) {
                points = POINT_GENERATOR::gen_points_grid((int)sqrt(gstate->futNumP), (int)sqrt(gstate->futNumP), p10, p11, p12, p13);
            }
            if (!dgui->state->genGrid) {
                points = POINT_GENERATOR::gen_points_square(gstate->futNumP, p10, p11, p12, p13);
            }
            delete t;
            delete td;
            delete ts;
            t = new Triangulation(points,points.size(),true);
            td = new TriangulationDrawer(t);
            ts = new TriangulationSaker(t,maxVel);
            t->whichTriangle();
            ts->initRandomVel();
            gstate->numP = t->vcount;
            gstate->numT = t->tcount;
        }
        if(dgui->state->centroidAll){
            t->centroidAll(dgui->state->angle);
            //std::cout << " -------------------------------- - " << std::endl;
            //t->addCentroids();
            td->genBuffers();
            dgui->state->centroidAll = false;
            dgui->state->numT = t->tcount;
            dgui->state->numP = t->vcount;
        }
		if(dgui->state->hastosave){
		    dgui->state->hastosave=false;
            save_mesh(t,"mesh.off");
		}
        glPointSize(dgui->state->PointSize);
        td->setColor(dgui->state->triangulation_color);
        if(dgui->state->ShowTriangulation)
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
