#include "State.h"
#include <stdexcept>

RunningState::RunningState(WorkflowInstance& i) : InstanceState(i) {}
    
void RunningState::suspend() {
    WorkflowInstance* i = instance;
    i->setState(new SuspendedState(*i));
    i->publish("InstanceSuspended");
}
    
void RunningState::complete() {
    WorkflowInstance* i = instance;
    i->setState(new CompletedState(*i));
    i->publish("InstanceCompleted");
}
    
void RunningState::cancel() {
    WorkflowInstance* i = instance;
    i->cancelOutstandingWork();
    i->setState(new CancelledState(*i));
    i->publish("InstanceCancelled");
} 
    
void RunningState::fail(const Issue&) {
    WorkflowInstance* i = instance;
    i->cancelOutstandingWork();
    i->setState(new FailedState(*i));
    i->publish("InstanceFailed");
}
    
bool RunningState::canCancel() const {
    return true;
}
    
std::string RunningState::getName() const {
    return "Running";
}
