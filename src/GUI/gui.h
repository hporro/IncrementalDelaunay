#include <GLFW/glfw3.h>
#include "imgui.h"

struct GUIState {
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
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