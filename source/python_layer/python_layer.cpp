#include "python_layer.h"
#include "../kv.h"


PYBIND11_EMBEDDED_MODULE(SimuDracoPython,m){
    m.doc() = "Simulador para draco";

    py::enum_<PythonGraphFlags::GraphFlags>(m,"GraphFlags",py::arithmetic())
        .value("None",PythonGraphFlags::None)
        .value("AutoFitX",PythonGraphFlags::AutoFitX)
        .value("AutoFitY",PythonGraphFlags::AutoFitY)
        .export_values();

    py::enum_<GraphType::PythonGraphTypes>(m,"GraphType")
        .value("Bars",GraphType::Bars)
        .value("Lines",GraphType::Lines)
        .export_values();
    
    py::class_<PythonGraphProperties>(m,"GraphProperties")
        .def(py::init<>())
        .def_readwrite("x_label",&PythonGraphProperties::x_label)
        .def_readwrite("y_label",&PythonGraphProperties::y_label)
        .def_readwrite("flags",&PythonGraphProperties::graphFlags);
    py::class_<PythonGraphWrapper>(m,"Graph")
        .def(py::init<>())
        .def_readwrite("name",&PythonGraphWrapper::name)
        .def_readwrite("graph_type",&PythonGraphWrapper::graphType)
        .def_readwrite("graph_function",&PythonGraphWrapper::graphUpdateFunction)
        .def_readwrite("graph_properties",&PythonGraphWrapper::properties);
    

};

std::map<std::string,std::string> PythonLayer::m_LoadedFilePerTab;
std::map<std::string,std::vector<std::string>> PythonLayer::m_LoadedTabsPerFile;
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
    
    std::vector<std::string> loadedTabs;
    
    for(auto& item : locals){
        if(py::type::of(item.second).equal(type)){
           
            py::object obj;
            std::string errorMsg = "";
            if(!PY_ASSERT(obj = item.second(),&errorMsg)){
                GuiLayer::AddErrorMsg(errorMsg);
                return false;
            }
            py::list my_list;
            if(!PY_ASSERT(my_list = py::globals()["get_public_variable_names"](obj),&errorMsg)){
                GuiLayer::AddErrorMsg(errorMsg);
                return false;
            }
            std::vector<std::string> my_vec;
            if(!PY_TRY_CAST(my_vec = my_list.cast<std::vector<std::string>>())){
                return false;
            }
            
            std::string tabName = "";
            if(!PY_TRY_CAST(tabName = py::str(item.first).cast<std::string>())){
                GuiLayer::AddErrorMsg("Couldn't convert class name to string");
                return false;
            }
            GuiTab& tab = GuiLayer::CreateTab(tabName);
            loadedTabs.push_back(tab.name);
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
            m_LoadedFilePerTab[tab.name] = filepath;
       }
       else {
           PY_CALL(py::globals()[item.first] = item.second);
       }
    }


    m_LoadedTabsPerFile[filepath] = std::move(loadedTabs);
    return true;
}

std::string PythonLayer::PythonLayer::GetVariableTypeName(py::object obj, std::string varName) {
    std::string type;
    std::string errMsg;
    if(!PY_TRY_CAST(type = py::str(py::globals()["__builtins__"].attr("type")(obj.attr(varName.c_str())).attr("__name__")).cast<std::string>(),&errMsg)){
        GuiLayer::AddErrorMsg(errMsg);
        return "";
    }


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
        tab.propertiesFunctions[varName] = [varName,showName](py::object thisObj){
            int var;
            
            if(!PY_TRY_CAST(var = thisObj.attr(py::str(varName)).cast<int>())){
                GuiLayer::AddErrorMsg("Variable '" + varName + "' was initialized as integer but was changed during runtime.\nPlease make sure the initialized variable keeps the same type at all times.\nRemoving variable from properties panel.");
                return false;
            }
            ImGui::InputInt(showName.c_str(),&var);
            thisObj.attr(py::str(varName)) = var;
            return true;
        };
        return true;
    }
    else if(varType == "float"){
        tab.propertiesFunctions[varName] = [varName,showName](py::object thisObj){
            double var;
            
            if(!PY_TRY_CAST(var = thisObj.attr(py::str(varName)).cast<double>())){
                GuiLayer::AddErrorMsg("Variable '" + varName + "' was initialized as float but was changed during runtime.\nPlease make sure the initialized variable keeps the same type at all times.\nRemoving variable from properties panel.");
                return false;
            }
            ImGui::InputDouble(showName.c_str(),&var);
            thisObj.attr(py::str(varName)) = var;
            return true;
        };
        return true;
    }
    else {
        return true;
    }
    


}

bool PythonLayer::HandleGraphs(py::object obj, std::string varName, GuiTab& tab) {
    PythonGraphWrapper wrapper;
    std::string errMsg;
    if(!PY_ASSERT(wrapper = obj.attr(varName.c_str()).cast<PythonGraphWrapper>(),&errMsg)){
        GuiLayer::AddErrorMsg(errMsg);
        GuiLayer::DeleteTab(tab.name);
        return false;
    }

    
    ImPlotAxisFlags flagsX = ImPlotAxisFlags_RangeFit;
    ImPlotAxisFlags flagsY = ImPlotAxisFlags_RangeFit;



    // setting up graph properties
    
    if(wrapper.properties.graphFlags & PythonGraphFlags::AutoFitX){
        flagsX |= ImPlotAxisFlags_AutoFit;
    }

    if(wrapper.properties.graphFlags & PythonGraphFlags::AutoFitY){
        flagsY |= ImPlotAxisFlags_AutoFit;
    }



    GuiGraphWrapper graphWrapper;
    graphWrapper.wrapper = wrapper;
    switch(wrapper.graphType){
    case GraphType::Bars:
        if(!wrapper.graphUpdateFunction(1,1).contains("x") && !wrapper.graphUpdateFunction(1,1).contains("y")){
            LOG_TO_USER("Graph update function for " << wrapper.name << " did not contain 'x' and 'y' values");
            return false;
        };
        graphWrapper.graphFunction = [=](std::map<std::string,std::vector<double>>& map){
            
            if(ImPlot::BeginPlot(wrapper.name.c_str())){
                ImPlot::SetupAxes(wrapper.properties.x_label.c_str(), wrapper.properties.y_label.c_str(), flagsX, flagsY);
                ImPlot::PlotBars(("##" + std::to_string(std::hash<std::string>()(wrapper.name))).c_str(),map["x"].data(),map["y"].data(),map["x"].size(),0.05f);

                ImPlot::EndPlot();
            }

        };
        tab.graphingFunctions[wrapper.name] = std::move(graphWrapper);
        break;
    case GraphType::Lines:
        
        if(!wrapper.graphUpdateFunction(1,1).contains("x") && !wrapper.graphUpdateFunction(1,1).contains("y")){
            LOG_TO_USER("Graph update function for '" << wrapper.name << "' did not contain 'x' and 'y' values");
            return false;
        };
        graphWrapper.graphFunction = [=](std::map<std::string,std::vector<double>>& map){

            if(ImPlot::BeginPlot(wrapper.name.c_str())){
                ImPlot::SetupAxes(wrapper.properties.x_label.c_str(), wrapper.properties.y_label.c_str(), flagsX,flagsY);
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

void PythonLayer::ReloadCurrentPythonFile() {
    const GuiTab& currentTab = GuiLayer::GetCurrentTab();

    if (currentTab.name == "None") {
        return;
    }

    std::string fileName = m_LoadedFilePerTab[currentTab.name];
    m_LoadedFilePerTab.erase(currentTab.name);

    for (auto& tabName : m_LoadedTabsPerFile[fileName]) {
        GuiLayer::DeleteTab(tabName);
    }

    m_LoadedTabsPerFile.erase(fileName);

    if (m_LoadedTabsPerFile.size() != 0) {
        GuiLayer::SetCurrentTab(m_LoadedFilePerTab.begin()->first);
    }
    else {
        GuiLayer::SetCurrentTab("None");
    }
    LoadPythonFile(fileName);

}

void PythonLayer::DeleteCurrentTab() {
    const GuiTab& currentTab = GuiLayer::GetCurrentTab();

    if (currentTab.name == "None") {
        return;
    }

    std::string fileName = m_LoadedFilePerTab[currentTab.name];
    m_LoadedFilePerTab.erase(currentTab.name);

    for (auto& tabName : m_LoadedTabsPerFile[fileName]) {
        GuiLayer::DeleteTab(tabName);
    }

    m_LoadedTabsPerFile.erase(fileName);

    if (m_LoadedTabsPerFile.size() != 0) {
        GuiLayer::SetCurrentTab(m_LoadedFilePerTab.begin()->first);
    }
    else {
        GuiLayer::SetCurrentTab("None");
    }
}
