#include "registry.h"
#include "registry_creates.h"
#include "registry_getters.h"
#include "registry_setters.h"
#include "window/window.h"
#include <future>

double Registry::m_DeltaTime = 0;
SimulationPropertiesStorage Registry::m_SimulationProperties;
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

    static double currentTime = 0;
    static double oldTime = 0;

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

        currentTime = glfwGetTime();
        m_DeltaTime = static_cast<double>(currentTime - oldTime) * m_SimulationProperties.timeScale;

        oldTime = currentTime;

        if(!m_SimulationProperties.paused){
            m_SimulationProperties.currentTime += m_DeltaTime;
        }
        
        
        glfwMakeContextCurrent(m_MainWindow.get()->m_ContextPointer);
        m_MainWindow.get()->BeginDrawState();
        
        for(auto& func : m_MainWindow.get()->m_PreDrawingLoopFuncs){
            func.second(*m_MainWindow.get());
        }
        for(auto& func : m_MainWindow.get()->m_DrawingLoopFuncs){
            func.second(*m_MainWindow.get());
        }
        for(auto& func : m_MainWindow.get()->m_PostDrawingLoopFuncs){
            func.second(*m_MainWindow.get());
        }
        m_MainWindow.get()->EndDrawState();

        std::unordered_map<std::string,std::unique_ptr<Window>>::iterator it = m_SubWindows.begin();
        while(it != m_SubWindows.end()){
            if(it->second.get()->IsOpen()){
                glfwMakeContextCurrent(it->second.get()->m_ContextPointer);
                it->second.get()->BeginDrawState();
                for(auto& func : it->second.get()->m_PreDrawingLoopFuncs){
                    func.second(*it->second.get());
                }
                for(auto& func : it->second.get()->m_DrawingLoopFuncs){
                    func.second(*it->second.get());
                }
                for(auto& func : it->second.get()->m_PostDrawingLoopFuncs){
                    func.second(*it->second.get());
                }
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
