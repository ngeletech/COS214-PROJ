#include "InstanceState.h"
#include "WorkflowInstance.h"
#include <stdexcept>

FailedState::FailedState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string FailedState::getName() const {
    return "Failed";
}  