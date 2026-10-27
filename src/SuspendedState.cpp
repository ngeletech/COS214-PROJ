#include "State.h"
#include <stdexcept>

SuspendedState::SuspendedState(WorkflowInstance& i) : InstanceState(i) {}
    
void SuspendedState::resume() {
    WorkflowInstance* i = instance;
    i->setState(new RunningState(*i));
    i->publish("InstanceResumed");
}
    
void SuspendedState::cancel() {
    WorkflowInstance* i = instance;
    i->setState(new CancelledState(*i));
    i->publish("InstanceCancelled");
}
    
bool SuspendedState::canCancel() const {
    return true;
}
    
std::string SuspendedState::getName() const {
    return "Suspended";
} 
