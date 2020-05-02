#include <GLFW/glfw3.h>
#include "imgui.h"
#include "../delaunay.h"
#include "../draw_triangulation.h"

struct GUIState {
    bool optionsActive = true;
    float triangulation_color[3] = {1.0f, 0.5f, 0.2f};
    double xpos, ypos;
    Triangulation t;
    TriangulationDrawer td;
    GUIState(){}
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