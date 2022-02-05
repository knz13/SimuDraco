#include "../kv.h"
#include "gui_layer.h"

ImGuiIO* GuiLayer::m_IO = nullptr;

void GuiLayer::Init(Window& win) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    m_IO = &ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(win.GetContextPointer(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    win.SetPreDrawingLoop([&](Window& window){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        CreateGraphPanel(win);

    });

     win.SetPostDrawingLoop([&](Window& window){
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });   
}

void GuiLayer::CreateGraphPanel(Window& win) {
    ImGui::Begin("Graphs",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetWindowSize(ImVec2(win.Properties().width/2,win.Properties().height/2));
    ImGui::SetWindowPos(ImVec2(win.Properties().width/2,win.Properties().height/2));

    ImGui::Text("Hello!");

    ImGui::End();
}
