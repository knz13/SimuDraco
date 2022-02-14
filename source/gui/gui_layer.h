#pragma once
#include "../global.h"

class Window;
class GuiLayer {
public:
    static bool Init(Window& win);
    static void Cleanup();

    static void AddErrorMsg(std::string msg);  
    static GuiTab& CreateTab(std::string name);
    static bool DeleteTab(std::string name);
    static void SetCurrentTab(std::string tabName);
    static const GuiTab& GetCurrentTab();

private:
    static void CreateGraphPanel(Window& win);
    static void UpdateGraphs();
    static void HandleGraphTimeSkip();
    static void CreateErrorMsgPanel(Window& win);

    static void CreatePropertiesPanel(Window& win);
    
    

    static std::vector<std::string> m_ErrorMsgs;
    static unsigned int m_NumberOfErrors;
    static std::string m_CurrentTab;
    static ImGuiIO* m_IO;
    static NFD::Guard m_FileDialogHandle;
    static std::unordered_map<std::string,GuiTab> m_Tabs;

};