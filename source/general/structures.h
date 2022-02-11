#pragma once
#include <iostream>
#include <unordered_map>
#include <map>
#include <functional>

#include "pybind11/pybind11.h"

namespace py = pybind11;

enum WindowFlag {

    None = 0,
    NotResizeable = 1,
    InitiallyMinimized = 2,
    NotFocused = 4,
    OpenGLDebugContext = 8,
    NotDecorated=16,
    FullScreen=32,
    CoreProfile=64,
    FocusOnShow=128
};

enum ShaderType {
    Vertex=0,
    Fragment,
};

struct ShaderProperties {
    std::string source;
    unsigned int shaderType;


};

struct WindowCreationProperties {
    int width=640;
    int height=480;
    std::string title="Base Title";
    uint32_t windowFlags = WindowFlag::None;
    int openGLVersionMajor = 4;
    int openGLVersionMinor = 0;
};

namespace GraphType {
    enum PythonGraphTypes {
        Bars=0,
        Lines=1
    };
};

struct PythonGraphWrapper {
    std::string name = "";
    GraphType::PythonGraphTypes graphType = GraphType::Bars;
    std::function<std::map<std::string,float>(py::object,float)> graphUpdateFunction = [](py::object obj,float deltaTime){return std::map<std::string,float>();};
};

struct GuiGraphWrapper {
    std::function<void()> graphFunction = [](){};
    std::map<std::string,std::vector<float>> graphData;
    PythonGraphWrapper wrapper;
};





struct GuiTab {

    std::string name= "";
    py::object objectReference;
    std::unordered_map<std::string,GuiGraphWrapper> graphingFunctions;
    std::unordered_map<std::string,std::function<void(py::object)>> propertiesFunctions;

};


class Window;
template<typename T>
struct WindowLoopSink {
    WindowLoopSink(std::unordered_map<uint32_t,std::function<T>>& sink);

    uint32_t Connect(std::function<T> windowFunc);
    bool Disconnect(uint32_t handle);

private:

    bool GetNewHandle();

    std::unordered_map<uint32_t,std::function<T>>& m_FuncRef;

    static std::unordered_map<void*,uint32_t> m_HandleCount;

};

template<typename T>
std::unordered_map<void*,uint32_t> WindowLoopSink<T>::m_HandleCount;

template<typename T>
uint32_t WindowLoopSink<T>::Connect(std::function<T> windowFunc) {
    uint32_t handle = GetNewHandle();
    m_FuncRef[handle] = std::move(windowFunc);
    return handle;
}

template<typename T>
bool WindowLoopSink<T>::Disconnect(uint32_t handle) {
    if(m_FuncRef.find(handle) != m_FuncRef.end()){
        m_FuncRef.erase(handle);
        return true;
    }
    return false;
}


template<typename T>
WindowLoopSink<T>::WindowLoopSink(std::unordered_map<uint32_t,std::function<T>>& sink) : m_FuncRef(sink){
    if(WindowLoopSink::m_HandleCount.find((void*)&sink) == WindowLoopSink::m_HandleCount.end()){
        WindowLoopSink::m_HandleCount[(void*)&sink] = 0;
    }
}

template<typename T>
bool WindowLoopSink<T>::GetNewHandle() {

    return WindowLoopSink::m_HandleCount[(void*)&m_FuncRef]++;
}
