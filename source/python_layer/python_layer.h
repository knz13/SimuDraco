#pragma once
#include "../global.h"

namespace py = pybind11;
class PythonLayer {
public:
    static void Init();
    static void Cleanup();
    static bool LoadPythonFile(std::string filepath);
    static void ReloadCurrentPythonFile();
    static void DeleteCurrentTab();

private:

    static std::string GetVariableTypeName(py::object obj,std::string varName);
    static bool HandleProperties(py::object obj, std::string varName,std::string varType,GuiTab& tab);
    static bool HandleGraphs(py::object obj, std::string varName,GuiTab& tab);


    static std::map<std::string,std::string> m_LoadedFilePerTab;
    static std::map<std::string,std::vector<std::string>> m_LoadedTabsPerFile;
    static py::scoped_interpreter m_Interpreter;
    static py::module m_Module;


};


