#include <iostream>
#include "kv.h"


int main(){
    WindowCreationProperties prop;
    prop.title = "Hey I'm a window!";
    prop.width = 1280;
    prop.height = 720;
    prop.windowFlags = WindowFlag::NotResizeable;

    Window& win = Registry::Create().MainWindow(prop);



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(win.GetContextPointer(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();


    win.SetPreDrawingLoop([&](Window& window){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    });

     win.SetPostDrawingLoop([&](Window& window){
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });    

    win.SetDrawingLoop([&](Window& window){
        
        ImGui::Begin("Hello");

            

        ImGui::End();


    });

    Registry::MainLoop();
    
    
}