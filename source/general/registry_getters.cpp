#include "registry_getters.h"
#include "registry.h"
#include "window/window.h"

Window& RegistryGetters::MainWindow() {
    if(Registry::m_MainWindow){
        return *Registry::m_MainWindow.get();
    }
    Registry::m_MainWindow = std::make_unique<Window>(WindowCreationProperties());
    return *Registry::m_MainWindow.get();
}

Window& RegistryGetters::SubWindow(std::string windowName) {
    if(!Registry::m_MainWindow){
        Registry::m_MainWindow = std::make_unique<Window>(WindowCreationProperties());
        DEBUG_WARN("Trying to get subwindow before initializing main one, prefer initializing first!");
    }

    if(Registry::m_SubWindows.find(windowName) != Registry::m_SubWindows.end()){
        return *Registry::m_SubWindows[windowName].get();
    }
    else {
        DEBUG_WARN("Subwindow with name: '" + windowName +"' was not found, creating new window...");
        Registry::m_SubWindows[windowName] = std::make_unique<Window>(WindowCreationProperties{.title=windowName});
        return *Registry::m_SubWindows[windowName].get();
    }
}

std::unordered_map<std::string,std::unique_ptr<Window>>& RegistryGetters::SubWindows() {
    return Registry::m_SubWindows;
}
