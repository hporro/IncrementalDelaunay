#include <GLFW/glfw3.h>
#include "imgui.h"
#include "../delaunay.h"
#include "../draw_triangulation.h"
#include "../point_generator.h"

class GUIState {
public:
    GUIState() {}
    bool optionsActive = true;
    float triangulation_color[3] = {0.565f, 0.0f, 0.686f};
    double xpos, ypos;
    int numP = 1000;
    int futNumP = numP;
    int numT;
    
    Triangulation* t;
    TriangulationDrawer* td;

    int PointSize = 5;
    bool ShowPoints = false;

    bool newTriagulationNeeded = false;
    bool genGrid = false;
    bool centroidAll = false;
    float offset[2] = {0.0,-0.3};
    float zoom = 0.6;
    float angle = 30.0;
};

class DelaunayGUI {
public:
    void init(GLFWwindow* window);
    GUIState* state;
    void draw();
    void render();
    GLFWwindow* window;
    ~DelaunayGUI();
};