#include "InstanceState.h"
#include "WorkflowInstance.h"
#include <stdexcept>

CompletedState::CompletedState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string CompletedState::getName() const {
    return "Completed";
} 