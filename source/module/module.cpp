#include "module.h"
#include "../general/structures.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/embed.h"
#include "pybind11/functional.h"
#include "pybind11/chrono.h"
#include "pybind11/complex.h"


PYBIND11_MODULE(SimuDracoPython,m){
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