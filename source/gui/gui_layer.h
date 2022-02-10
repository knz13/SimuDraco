#pragma once
#include "../global.h"

class Window;
class GuiLayer {
public:
    static bool Init(Window& win);
    static void Cleanup();

    static bool SetCurrentGraph(std::string graphName);

private:
    static void CreateGraphPanel(Window& win);
    static void CreatePropertiesPanel(Window& win);

    static std::unordered_map<std::string,std::vector<std::function<void()>>> m_Graphs;
    static std::string m_CurrentGraphName;
    static ImGuiIO* m_IO;
    

};