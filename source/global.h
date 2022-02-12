#pragma once
#include <iostream>
#include <future>
#include <optional>
#include <filesystem>
#include <format>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/embed.h"
#include "pybind11/functional.h"
#include "pybind11/chrono.h"
#include "pybind11/complex.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "implot.h"
#include "misc/cpp/imgui_stdlib.h"
#include "thread_pool.hpp"
#include "nfd.hpp"
#include "general/structures.h"
#include "general/color.h"

using namespace std;


#define GL_SIZEOF(x) TestSize(x)


static unsigned int TestSize(unsigned int dataType) {
    switch (dataType) {
    case GL_FLOAT:
        return sizeof(float);
    case GL_UNSIGNED_INT:
        return sizeof(unsigned int);
    case GL_UNSIGNED_BYTE:
        return sizeof(unsigned char);
    case GL_BYTE:
        return sizeof(char);
    case GL_INT:
        return sizeof(int);
    default:
        return 0;
    }
}


#define LOG_TO_USER(x) cout << "LOG: " << x << endl <<  "At line: "<< __LINE__ << endl << "In file: " << __FILE__ << endl


#ifdef NDEBUG
    #define DEBUG_LOG(x)
    #define DEBUG_WARN(x)
    #define DEBUG_ERROR(x)
#else
    #define DEBUG_LOG(x) cout << "LOG: " << x << endl <<  "At line: "<< __LINE__ << endl << "In file: " << __FILE__ << endl
    #define DEBUG_WARN(x) cout << "WARNING: " << x << endl <<  "At line: "<< __LINE__ << endl << "In file: " << __FILE__ << endl
    #define DEBUG_ERROR(x) cout << "ERROR! -> " << x  << endl <<  "At line: "<< __LINE__ << endl << "In file: " << __FILE__ << endl; __debugbreak()
#endif

static void ClearGLErrors(){
    while(glGetError() != GL_NO_ERROR){
        ;
    }
}

static bool GetGLError(int line,std::string file){
    GLenum code = glGetError();
    if(code != GL_NO_ERROR){
        cout << "OpenGL error '" << gluErrorString(code) << "' \nAt line: " << line << " \nIn file: " << file << endl;
        return true;
    }
    return false;
}

#ifdef NDEBUG
    #define GL_CALL(x) x 
    
#else
    #define GL_CALL(x) ClearGLErrors(); x; if(GetGLError(__LINE__,__FILE__)) {__debugbreak();}
    
#endif



#define PY_CALL(x) try { x; } catch(py::error_already_set &e){std::cout << e.what() << std::endl;}
#define PY_ASSERT(x) [&](){try { x; return true; } catch(py::error_already_set &e){DEBUG_LOG(e.what()); return false;}}()




