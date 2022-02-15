#include "gui_layer.h"
#include "../kv.h"
#include "imgui/imgui.h"

ImGuiIO* GuiLayer::m_IO = nullptr;
NFD::Guard GuiLayer::m_FileDialogHandle;
std::unordered_map<std::string,GuiTab> GuiLayer::m_Tabs = {{"None",GuiTab()}};
std::string GuiLayer::m_CurrentTab = "None";
std::vector<std::string> GuiLayer::m_ErrorMsgs;
unsigned int GuiLayer::m_NumberOfErrors = 0;

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
        CreateErrorMsgPanel(win);
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
    ImGui::Begin("Graphs",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowSize(ImVec2(4*win.Properties().width/6,win.Properties().height));
    ImGui::SetWindowPos(ImVec2(0,0));

    for(auto& graphWrapper : m_Tabs[m_CurrentTab].graphingFunctions){
        graphWrapper.second.graphFunction(graphWrapper.second.graphData);
    }


    

    ImGui::End();
}



void GuiLayer::CreatePropertiesPanel(Window& win) {
    static double pausedSimulationTime = 0;


    ImGui::Begin("Properties",NULL,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::SetWindowSize(ImVec2(2*win.Properties().width/6,win.Properties().height));
    ImGui::SetWindowPos(ImVec2(4*win.Properties().width/6,0));

    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("Menu")){
            
            if(ImGui::MenuItem("Add Python File")){
                m_NumberOfErrors = 0;
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
            m_NumberOfErrors = 0;
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

    ImGui::BulletText("Simulation Time Scale");
    ImGui::SliderFloat("##SimulationTimeScale",&Registry::m_SimulationProperties.timeScale,0,1);

    ImGui::BulletText("Current Simulation Time");
    if(!Registry::m_SimulationProperties.paused){
        ImGui::BulletText(std::to_string(Registry::m_SimulationProperties.currentTime).c_str());
    }
    else {
        float time = (float)Registry::m_SimulationProperties.currentTime;
        ImGui::SliderFloat("##CurrentSimulationTime",&time,0,(float)pausedSimulationTime);
        Registry::m_SimulationProperties.currentTime = time;
    }

    if(m_Tabs[m_CurrentTab].propertiesFunctions.size() > 0){
        ImGui::BulletText("Script Variables:");
    }
    std::string errorMsg;
    auto it = m_Tabs[m_CurrentTab].propertiesFunctions.begin();
    while(it != m_Tabs[m_CurrentTab].propertiesFunctions.end()){
        if(!it->second(m_Tabs[m_CurrentTab].objectReference)){
            it = m_Tabs[m_CurrentTab].propertiesFunctions.erase(it);
        }
        else {
            it++;
        }
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
        std::string errorMsg;
        if(!PY_ASSERT(dictWithGraphData = graphWrapper.second.wrapper.graphUpdateFunction(Registry::Get().DeltaTime(),Registry::Get().SimulationProperties().currentTime),&errorMsg)){
            PythonLayer::DeleteCurrentTab();
            GuiLayer::AddErrorMsg(errorMsg);
            Registry::m_SimulationProperties.currentTime = 0;
            Registry::m_SimulationProperties.paused = true;
            break;
        }

        std::map<std::string,double> mapWithData;
        if(!PY_TRY_CAST((mapWithData = dictWithGraphData.cast<std::map<std::string,double>>()))){
            stringstream ss;
            ss << "Result of function from graph: " << graphWrapper.first << " was not equal to a dict with string keys and double values";
            GuiLayer::AddErrorMsg(ss.str());
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

void GuiLayer::AddErrorMsg(std::string msg) {
    m_ErrorMsgs.push_back(msg);
    m_NumberOfErrors++;
}

void GuiLayer::CreateErrorMsgPanel(Window& win) {
    std::string windowName;

    if (m_NumberOfErrors > 0) {
        windowName = "Error Panel: " + to_string(m_NumberOfErrors) + " New Errors!";
    }
    else {
        windowName = "Error Panel: No Errors";
    }

    ImGui::Begin(windowName.c_str(),NULL,ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SetWindowCollapsed(true,ImGuiCond_Once);
    ImGui::SetWindowSize(ImVec2(4*win.Properties().width/6,win.Properties().height/2));

    if (ImGui::IsWindowCollapsed()) {
        ImGui::SetWindowPos(ImVec2(0, win.Properties().height));
    }
    else {
        ImGui::SetWindowPos(ImVec2(0, win.Properties().height / 2));
    }
    

    for (auto& text : m_ErrorMsgs) {
        ImGui::BulletText(("ERROR: " + text).c_str());
    }


    ImGui::End();
}
