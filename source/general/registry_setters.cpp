#include "registry_setters.h"
#include "registry.h"

void RegistrySetters::SimulationPauseState(bool state) {
    Registry::m_SimulationProperties.paused = state;
}
