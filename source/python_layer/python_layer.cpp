#include "python_layer.h"
#include "../module.h"

py::scoped_interpreter PythonLayer::m_Interpreter;
py::module PythonLayer::m_Module;
void PythonLayer::Init() {

    m_Module = py::module::import("SimuDracoPython");

    PY_CALL(py::exec(R"(

def get_public_func_names(variable):
    useful_funcs = []
    for func in dir(variable):
        if callable(getattr(variable,func)) and not func.endswith("__") and not func.startswith("_"):
            useful_funcs.append(func)
    return useful_funcs

    )"));

    PY_CALL(py::exec(R"(

def get_public_variable_names(variable):
    useful_variables = []
    for m_var in vars(variable):
        if not m_var.startswith("_"):
            useful_variables.append(m_var)
    return useful_variables

    )"));

}

bool PythonLayer::PythonLayer::LoadPythonFile(std::string filepath) {
    py::dict locals;
    py::eval_file(filepath,py::globals(),locals);


    py::object type = py::eval("type");

    int index = 0;
    for(auto& item : locals){
       if(py::type::of(item.second).equal(type)){
           
            py::object obj;
            PY_ASSERT(obj = item.second());
            py::list my_list;
            PY_ASSERT(my_list = py::globals()["get_public_func_names"](obj));
            std::vector<std::string> my_vec;
            PY_ASSERT(my_vec = my_list.cast<std::vector<std::string>>());
            std::unordered_map<std::string,int> my_map;
            PY_CALL(
                my_map = py::cast<decltype(my_map)>(obj.attr("plot_bars_1")());
            );
            
           



           index++;
       }
    }

    return true;
}
