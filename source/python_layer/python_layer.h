#pragma once
#include "../global.h"

namespace py = pybind11;
class PythonLayer {
public:
    static void Init();
    static bool LoadPythonFile(std::string filepath);

private:
    static py::scoped_interpreter m_Interpreter;
    static py::module m_Module;
};


