#pragma once
#include "../global.h"

namespace py = pybind11;
class PythonLayer {
public:
    static void Init();
    

private:
    static py::scoped_interpreter m_Interpreter;
};


