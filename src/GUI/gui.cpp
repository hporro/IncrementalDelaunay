#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>

#include <GL/gl3w.h>  
#include <GLFW/glfw3.h>

#include "gui.h"

void DelaunayGUI::init(GLFWwindow* window){
    this->window = window;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void DelaunayGUI::render(){
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DelaunayGUI::draw(){
    ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Options",&state->optionsActive,ImGuiWindowFlags_MenuBar);
            ImGui::Text("Triangulation visual options");
            ImGui::SliderFloat("Zoom", &state->zoom,0.1,10.0);
            ImGui::SliderFloat2("Offset", state->offset, -10.0, 10.0);
            ImGui::ColorEdit3("Color",state->triangulation_color);
            ImGui::Checkbox("Show Points",&state->ShowPoints);
            ImGui::InputInt("Point Size",&state->PointSize);
            ImGui::Text("Make triangulation options");
            ImGui::InputInt("Number of points",&state->futNumP);
            if(ImGui::Button("New Triangulation")){
                state->newTriagulationNeeded = true;
            }
            if(ImGui::Button("Centroid All")){
                state->centroidAll = true;
            }
            ImGui::Checkbox("Points in grid", &state->genGrid);
            ImGui::End();
        }
        {
            ImGui::Begin("Triangulation Info",&state->optionsActive);
            ImGui::Text("Number of Vertices: %d",state->numP);
            ImGui::Text("Number of Triangles: %d",state->numT);
            ImGui::End();
        }
        ImGui::Render();
}

DelaunayGUI::~DelaunayGUI(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}