#include <iostream>
#include "kv.h"

namespace py = pybind11;

int main(){
    
    WindowCreationProperties prop;
    prop.title = "Hey I'm a window!";
    prop.width = 1280;
    prop.height = 720;

    Window& win = Registry::Create().MainWindow(prop);
    
    PythonLayer::Init();
    GuiLayer::Init(win);

    Registry::MainLoop();
    
    GuiLayer::Cleanup();
    
}