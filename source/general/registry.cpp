#include "registry.h"
#include "registry_creates.h"
#include "registry_getters.h"
#include "registry_setters.h"
#include "window/window.h"
#include <future>


std::unique_ptr<Window> Registry::m_MainWindow;
std::unordered_map<std::string,std::unique_ptr<Window>> Registry::m_SubWindows;

template<typename T>
bool Registry::IsFutureDone(std::future<T>& fut) {
    return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

RegistrySetters Registry::Set() {
    return RegistrySetters();
}

RegistryGetters Registry::Get() {
    return RegistryGetters();
}

RegistryCreates Registry::Create() {
    return RegistryCreates();
}
void Registry::MainLoop() {
    if(!m_MainWindow){
        DEBUG_ERROR("Calling main window function without any window defined!");
        return;
    }

    glfwSetWindowCloseCallback(m_MainWindow.get()->m_ContextPointer,[](GLFWwindow* windowPointer){
        for(auto& window : Registry::Get().SubWindows()){
            glfwSetWindowShouldClose(window.second.get()->GetContextPointer(),GL_TRUE);
        }
    });

    
    while(m_MainWindow.get()->IsOpen()){


        glfwMakeContextCurrent(m_MainWindow.get()->m_ContextPointer);
        m_MainWindow.get()->BeginDrawState();
        
        m_MainWindow.get()->m_PreDrawingLoop(*m_MainWindow.get());
        m_MainWindow.get()->m_DrawingLoop(*m_MainWindow.get());
        m_MainWindow.get()->m_PostDrawingLoop(*m_MainWindow.get());
        
        m_MainWindow.get()->EndDrawState();

        std::unordered_map<std::string,std::unique_ptr<Window>>::iterator it = m_SubWindows.begin();
        while(it != m_SubWindows.end()){
            if(it->second.get()->IsOpen()){
                glfwMakeContextCurrent(it->second.get()->m_ContextPointer);
                it->second.get()->BeginDrawState();
                it->second.get()->m_DrawingLoop(*it->second.get());
                it->second.get()->EndDrawState();
                it++;
            }
            else {
                it = m_SubWindows.erase(it);
            }
        }    


    }

}

RegistryDeleters Registry::Delete() {
    return RegistryDeleters();
}
