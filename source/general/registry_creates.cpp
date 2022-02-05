#include "registry_creates.h"
#include "registry.h"
#include "window/window.h"



Window& RegistryCreates::MainWindow(WindowCreationProperties prop) {

    // Clearing the sub windows if any are found
    if(Registry::m_MainWindow){
        DEBUG_WARN("Main window was already defined, returning it...");
        return *Registry::m_MainWindow.get();
    }

    Registry::m_MainWindow = std::make_unique<Window>(prop);
    return *Registry::m_MainWindow.get();
}

Window& RegistryCreates::SubWindow(std::string windowName, WindowCreationProperties prop) {
    prop.windowFlags &= WindowFlag::NotDecorated;

    if(!Registry::m_MainWindow) {
        DEBUG_WARN("Creating subwindow with name '" +windowName +  "' before main one, prefer otherwise!");
        Registry::m_MainWindow = std::make_unique<Window>(WindowCreationProperties());
    }

    if(Registry::m_SubWindows.find(windowName) != Registry::m_SubWindows.end()){
        DEBUG_WARN("SubWindow with name '" + windowName +  "' was already created, returning it.");
        return *Registry::m_SubWindows[windowName].get();
    }

    Registry::m_SubWindows[windowName] = std::make_unique<Window>(prop);

    return *Registry::m_SubWindows[windowName].get();

}


