#pragma once
#include "../global.h"


class Window;
class RegistryGetters {
public:
    Window& MainWindow();
    Window& SubWindow(std::string windowName);
    std::unordered_map<std::string,std::unique_ptr<Window>>& SubWindows();
    



};