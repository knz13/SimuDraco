#pragma once

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