#include "State.h"
#include <stdexcept>

FailedState::FailedState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string FailedState::getName() const {
    return "Failed";
}  
