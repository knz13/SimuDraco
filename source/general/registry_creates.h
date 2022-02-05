#pragma once
#include "../global.h"

class Window;
class RegistryCreates {
public:
    Window& MainWindow(WindowCreationProperties prop);

    Window& SubWindow(std::string windowName,WindowCreationProperties prop);

    

};