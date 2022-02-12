#include "python_layer.h"
#include "../kv.h"


PYBIND11_EMBEDDED_MODULE(SimuDracoPython,m){
    m.doc() = "Simulador para draco";

    py::enum_<GraphType::PythonGraphTypes>(m,"GraphType")
        .value("Bars",GraphType::Bars)
        .value("Lines",GraphType::Lines)
        .export_values();
    
    py::class_<PythonGraphWrapper>(m,"Graph")
        .def(py::init<>())
        .def_readwrite("name",&PythonGraphWrapper::name)
        .def_readwrite("graph_type",&PythonGraphWrapper::graphType)
        .def_readwrite("graph_function",&PythonGraphWrapper::graphUpdateFunction);
    

};


py::scoped_interpreter PythonLayer::m_Interpreter;
py::module PythonLayer::m_Module;
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

    
    PY_CALL(py::eval_file(filepath,py::globals(),locals));

    PY_CALL(std::cout << py::str(locals) << std::endl);

    py::object type = py::eval("type");
    

    
    for(auto& item : locals){
        if(py::type::of(item.second).equal(py::type::of(py::globals()["__builtins__"]))){
            PY_CALL(py::globals()[item.first] = item.second);
            continue;
        };
        if(py::type::of(item.second).equal(type)){
           
            py::object obj;
            if(!PY_ASSERT(obj = item.second())){
                return false;
            }
            py::list my_list;
            if(!PY_ASSERT(my_list = py::globals()["get_public_variable_names"](obj))){
                return false;
            }
            std::vector<std::string> my_vec;
            if(!PY_ASSERT(my_vec = my_list.cast<std::vector<std::string>>())){
                return false;
            }
            
            
            GuiTab& tab = GuiLayer::CreateTab(py::str(item.first).cast<std::string>());
            tab.objectReference = obj;
            

            for(auto& variable : my_vec){
                std::string typeName = GetVariableTypeName(obj,variable);
                if(typeName == "Graph"){
                    HandleGraphs(obj,variable,tab);
                }
                else {
                    HandleProperties(obj,variable,typeName,tab);
                }
            }
            GuiLayer::SetCurrentTab(tab.name);
       }
    }

    return true;
}

std::string PythonLayer::PythonLayer::GetVariableTypeName(py::object obj, std::string varName) {
    std::string type;

    PY_CALL(type = py::str(py::globals()["__builtins__"].attr("type")(obj.attr(varName.c_str())).attr("__name__")).cast<std::string>());


    return type;
}   

bool PythonLayer::HandleProperties(py::object obj, std::string varName,std::string varType, GuiTab& tab) {
    
    std::string showName = varName;
    std::string test = showName.substr(0,2);
    if(showName.substr(0,2) == "m_"){
        showName = showName.substr(2,showName.size() - 2);
    }
    std::replace(showName.begin(),showName.end(),'_',' ');



    if(varType == "int"){
        tab.propertiesFunctions[varName] = [=](py::object thisObj){
            int var = thisObj.attr(py::str(varName)).cast<int>();
            ImGui::InputInt(showName.c_str(),&var);
            thisObj.attr(py::str(varName)) = var;
        };
        return true;
    }
    else if(varType == "float"){
        tab.propertiesFunctions[varName] = [=](py::object thisObj){
            float var = thisObj.attr(py::str(varName)).cast<float>();
            ImGui::InputFloat(showName.c_str(),&var);
            thisObj.attr(py::str(varName)) = var;
        };
        return true;
    }
    else {
        return true;
    }
    


}

bool PythonLayer::HandleGraphs(py::object obj, std::string varName, GuiTab& tab) {
    PythonGraphWrapper wrapper;
    if(!PY_ASSERT(wrapper = obj.attr(varName.c_str()).cast<PythonGraphWrapper>())){
        GuiLayer::DeleteTab(tab.name);
        return false;
    }
    GuiGraphWrapper graphWrapper;
    graphWrapper.wrapper = wrapper;
    switch(wrapper.graphType){
    case GraphType::Bars:
        if(!wrapper.graphUpdateFunction(0,0).contains("x") && !wrapper.graphUpdateFunction(0,0).contains("y")){
            LOG_TO_USER("Graph update function for " << wrapper.name << " did not contain 'x' and 'y' values");
            return false;
        }
        graphWrapper.graphFunction = [=](std::map<std::string,std::vector<float>>& map){
            if(ImPlot::BeginPlot(wrapper.name.c_str())){
                
                ImPlot::PlotBars(("##" + std::to_string(std::hash<std::string>()(wrapper.name))).c_str(),map["x"].data(),map["y"].data(),map["x"].size(),2);

                ImPlot::EndPlot();
            }

        };
        tab.graphingFunctions[wrapper.name] = std::move(graphWrapper);
        break;
    case GraphType::Lines:
        if(!wrapper.graphUpdateFunction(0,0).contains("x") && !wrapper.graphUpdateFunction(0,0).contains("y")){
            LOG_TO_USER("Graph update function for '" << wrapper.name << "' did not contain 'x' and 'y' values");
            return false;
        }
        graphWrapper.graphFunction = [=](std::map<std::string,std::vector<float>>& map){
            
            if(ImPlot::BeginPlot(wrapper.name.c_str())){
                
                ImPlot::PlotLine(("##" + std::to_string(std::hash<std::string>()(wrapper.name))).c_str(),map["x"].data(),map["y"].data(),map["x"].size());

                ImPlot::EndPlot();
            }

        };
        tab.graphingFunctions[wrapper.name] = std::move(graphWrapper);
        break;
    }
    return true;
}

void PythonLayer::Cleanup() {
    
}
