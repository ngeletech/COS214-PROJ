#include "State.h"
#include <stdexcept>

CreatedState::CreatedState(WorkflowInstance& i) : InstanceState(i) {}
    
void CreatedState::start() {
    WorkflowInstance* i = instance;
    i->setState(new RunningState(*i));
    i->publish("InstanceStarted");
}
    
void CreatedState::cancel() {
    WorkflowInstance* i = instance;
    i->setState(new CancelledState(*i));
    i->publish("InstanceCancelled");
}
    
bool CreatedState::canCancel() const {
    return true;
}
    
std::string CreatedState::getName() const {
    return "Created";
}
