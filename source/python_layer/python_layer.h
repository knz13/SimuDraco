#pragma once
#include "../global.h"

namespace py = pybind11;
class PythonLayer {
public:
    static void Init();
    static void Cleanup();
    static bool LoadPythonFile(std::string filepath);

private:

    static std::string GetVariableTypeName(py::object obj,std::string varName);
    static bool HandleProperties(py::object obj, std::string varName,std::string varType,GuiTab& tab);
    static bool HandleGraphs(py::object obj, std::string varName,GuiTab& tab);



    static py::scoped_interpreter m_Interpreter;
    static py::module m_Module;


};


