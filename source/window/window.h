#pragma once 
#include "../global.h"
#include <functional>

class Window {
public:

    Window(WindowCreationProperties prop);
    
    ~Window();

    bool IsOpen();
    
    WindowLoopSink<void(Window&)> PreDrawingLoop();
    WindowLoopSink<void(Window&)> DrawingLoop();
    WindowLoopSink<void(Window&)> PostDrawingLoop();
    WindowLoopSink<void(Window&)> Closing();

    void SetClearColor(Color color);

    const WindowCreationProperties& Properties();
    GLFWwindow* GetContextPointer();

private:
    void BeginDrawState();
    void EndDrawState();

    
    WindowCreationProperties m_Properties;
    GLFWwindow* m_ContextPointer=nullptr;

    std::unordered_map<uint32_t,std::function<void(Window&)>> m_PreDrawingLoopFuncs;
    std::unordered_map<uint32_t,std::function<void(Window&)>> m_DrawingLoopFuncs;
    std::unordered_map<uint32_t,std::function<void(Window&)>> m_PostDrawingLoopFuncs;
    std::unordered_map<uint32_t,std::function<void(Window&)>> m_ClosingCallbackFuncs;
    Color m_ClearColor = Color::Black;
    

    friend class Registry;

};