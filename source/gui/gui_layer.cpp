#include "gui_layer.h"
#include "../kv.h"
#include "imgui/imgui.h"

ImGuiIO* GuiLayer::m_IO = nullptr;
NFD::Guard GuiLayer::m_FileDialogHandle;
std::unordered_map<std::string,GuiTab> GuiLayer::m_Tabs = {{"None",GuiTab()}};
std::string GuiLayer::m_CurrentTab = "None";

bool GuiLayer::Init(Window& win) {
    if(m_IO != nullptr){
        return true;
    }
    
    glfwSetWindowSizeCallback(Registry::Get().MainWindow().GetContextPointer(),[](GLFWwindow* win,int width,int height){
        Registry::Set().SimulationPauseState(true);
    });

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
        if(!Registry::Get().SimulationProperties().paused){
            UpdateGraphs();
        }
        
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
    m_Tabs.clear();
    
}

void GuiLayer::CreateGraphPanel(Window& win) {
    ImGui::Begin("Graphs",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::SetWindowSize(ImVec2(4*win.Properties().width/6,win.Properties().height));
    ImGui::SetWindowPos(ImVec2(0,0));

    for(auto& graphWrapper : m_Tabs[m_CurrentTab].graphingFunctions){
        graphWrapper.second.graphFunction(graphWrapper.second.graphData);
    }


    

    ImGui::End();
}



void GuiLayer::CreatePropertiesPanel(Window& win) {
    static float pausedSimulationTime = 0;


    ImGui::Begin("Properties",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(2*win.Properties().width/6,win.Properties().height));
    ImGui::SetWindowPos(ImVec2(4*win.Properties().width/6,0));

    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("Menu")){
            
            if(ImGui::MenuItem("Add Python File")){
                NFD::UniquePath outPath;
                nfdfilteritem_t filter[] = {{"Python File","py"}};
                nfdresult_t result = NFD::OpenDialog(outPath,filter,1,std::filesystem::current_path().string().c_str());
                if(result == NFD_OKAY){
                    PythonLayer::LoadPythonFile(outPath.get());
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if(Registry::m_SimulationProperties.paused){
        if(ImGui::Button("Play")){
            Registry::m_SimulationProperties.paused = false;
            HandleGraphTimeSkip();  
        }
    }
    else {
        if(ImGui::Button("Pause")){
            pausedSimulationTime = Registry::m_SimulationProperties.currentTime;
            Registry::m_SimulationProperties.paused = true;
        }
    }
    
    if (m_CurrentTab != "None") {
        if (ImGui::Button("Reload Python File")) {
            PythonLayer::ReloadCurrentPythonFile();
        }
    }


    ImGui::BulletText("Current Simulation Time");
    if(!Registry::m_SimulationProperties.paused){
        ImGui::BulletText(std::to_string(Registry::m_SimulationProperties.currentTime).c_str());
    }
    else {
        ImGui::SliderFloat("##CurrentSimulationTime",&Registry::m_SimulationProperties.currentTime,0,pausedSimulationTime);
        
    }

    if(m_Tabs[m_CurrentTab].propertiesFunctions.size() > 0){
        ImGui::BulletText("Script Variables:");
    }
    for(auto& func : m_Tabs[m_CurrentTab].propertiesFunctions){
        func.second(m_Tabs[m_CurrentTab].objectReference);
    }
    


    ImGui::End();
}


GuiTab& GuiLayer::CreateTab(std::string name) {
    m_Tabs[name] = GuiTab();
    m_Tabs[name].name = name;
    return m_Tabs[name];    
}
bool GuiLayer::DeleteTab(std::string name) {
    if(m_Tabs.find(name) == m_Tabs.end()){
        return false;
    }
    m_Tabs.erase(name);
    return true;
}

void GuiLayer::SetCurrentTab(std::string tabName) {
    if(m_Tabs.find(tabName) != m_Tabs.end()){
        m_CurrentTab = tabName;
    }   
}

void GuiLayer::UpdateGraphs() {
    
    for(auto& graphWrapper : m_Tabs[m_CurrentTab].graphingFunctions){
        py::object dictWithGraphData;
        if(!PY_ASSERT(dictWithGraphData = graphWrapper.second.wrapper.graphUpdateFunction(Registry::Get().DeltaTime(),Registry::Get().SimulationProperties().currentTime))){
            PythonLayer::DeleteCurrentTab();
            break;
        }

        std::map<std::string,float> mapWithData;
        if(!PY_ASSERT((mapWithData = dictWithGraphData.cast<std::map<std::string,float>>()))){
            LOG_TO_USER("Result of function from graph: " << graphWrapper.first << " was not equal to a dict with string keys and float values");
            continue;
        }

        graphWrapper.second.graphUpdateTimeLog.push_back(std::make_pair<>(Registry::m_SimulationProperties.currentTime,mapWithData));
        for(auto& value : mapWithData){
            graphWrapper.second.graphData[value.first].push_back(value.second);
        }

    }
}

void GuiLayer::HandleGraphTimeSkip() {
    for(auto& graphWrapper : m_Tabs[m_CurrentTab].graphingFunctions){
        if(Registry::m_SimulationProperties.currentTime == 0){
            graphWrapper.second.graphData.clear();
            graphWrapper.second.graphUpdateTimeLog.clear();
        }


        auto it = graphWrapper.second.graphUpdateTimeLog.begin();

        while(it != graphWrapper.second.graphUpdateTimeLog.end()){
            if(it->first > Registry::m_SimulationProperties.currentTime){
                break;
            }
            it++;
        }
        if(it != graphWrapper.second.graphUpdateTimeLog.begin()){
            graphWrapper.second.graphUpdateTimeLog.erase(it,graphWrapper.second.graphUpdateTimeLog.end());
        }

        for(auto& vec : graphWrapper.second.graphData){
            size_t vecSize = vec.second.size();
            vec.second.clear();
            vec.second.reserve(graphWrapper.second.graphUpdateTimeLog.size());
        }
        
        
        for(auto& dict : graphWrapper.second.graphUpdateTimeLog){
            for(auto& dict_val : dict.second){
                graphWrapper.second.graphData[dict_val.first].push_back(dict_val.second);
            }
        }
    }
}

const GuiTab& GuiLayer::GetCurrentTab() {
    return m_Tabs[m_CurrentTab];
}
