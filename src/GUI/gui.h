#include <GLFW/glfw3.h>
#include "imgui.h"

struct GUIState {
    bool optionsActive = true;
    float triangulation_color[3] = {1.0f, 0.5f, 0.2f};
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