#pragma once
#include "../global.h"

class Window;
class GuiLayer {
public:
    static void Init(Window& win);

private:
    static void CreateGraphPanel(Window& win);
    static ImGuiIO* m_IO;

};