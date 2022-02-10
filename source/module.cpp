#include "pybind11/pybind11.h"
#include "general/structures.h"

namespace py = pybind11;

PYBIND11_MODULE(SimuDracoPython,m){
    m.doc() = "Simulador para draco";


};