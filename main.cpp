#include <iostream>
#include "kv.h"


int main(){
    WindowCreationProperties prop;
    prop.title = "Hey I'm a window!";
    prop.width = 1280;
    prop.height = 720;
    prop.windowFlags = WindowFlag::NotResizeable;

    Window& win = Registry::Create().MainWindow(prop);



    GuiLayer::Init(win);

    win.SetDrawingLoop([&](Window& window){
        
        


    });

    Registry::MainLoop();
    
    
}