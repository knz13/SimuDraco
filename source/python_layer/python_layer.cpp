#include "python_layer.h"

py::scoped_interpreter PythonLayer::m_Interpreter;
void PythonLayer::Init() {
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
           PY_ASSERT(std::cout << py::str(my_list) << std::endl);


           index++;
       }
    }

    return true;
}
