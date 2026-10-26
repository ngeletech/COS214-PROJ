#include "InstanceState.h"
#include "WorkflowInstance.h"
#include <stdexcept>

CancelledState::CancelledState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string CancelledState::getName() const {
    return "Cancelled";
} 

