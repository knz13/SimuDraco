#include "../kv.h"
#include "gui_layer.h"

ImGuiIO* GuiLayer::m_IO = nullptr;
std::unordered_map<std::string,std::vector<std::function<void()>>> GuiLayer::m_Graphs;
std::string GuiLayer::m_CurrentGraphName;

bool GuiLayer::Init(Window& win) {
    if(m_IO != nullptr){
        return true;
    }
    
    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    m_IO = &ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(win.GetContextPointer(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    win.PreDrawingLoop().Connect([&](Window& window){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        CreateGraphPanel(win);
        CreatePropertiesPanel(win);

    });

    win.PostDrawingLoop().Connect([&](Window& window){
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return true; 
}

void GuiLayer::Cleanup() {
    
}

void GuiLayer::CreateGraphPanel(Window& win) {
    ImGui::Begin("Graphs",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(4*win.Properties().width/6,win.Properties().height));
    ImGui::SetWindowPos(ImVec2(0,0));

    


    

    ImGui::End();
}

bool GuiLayer::SetCurrentGraph(std::string graphName) {
    if(m_Graphs.find(graphName) != m_Graphs.end()){
        m_CurrentGraphName = graphName;
        return true;
    }
    return false;
}

void GuiLayer::CreatePropertiesPanel(Window& win) {
    ImGui::Begin("Properties",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(2*win.Properties().width/6,win.Properties().height));
    ImGui::SetWindowPos(ImVec2(4*win.Properties().width/6,0));

    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("Menu")){
            
            if(ImGui::MenuItem("Hello")){
                
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    


    ImGui::End();
}