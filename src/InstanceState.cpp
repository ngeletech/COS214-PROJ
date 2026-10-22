#include "InstanceState.h"
#include "WorkflowInstance.h"
#include <stdexcept>

//InstanceState - every action is refused unless a concrete state overrides it
InstanceState::InstanceState(WorkflowInstance& i) : instance(&i) {}
        
void InstanceState::start(){
    throw std::logic_error("Cannot start while " + getName());
}
    
void InstanceState::suspend(){
    throw std::logic_error("Cannot suspend while " + getName());
} 
    
void InstanceState::resume(){
    throw std::logic_error("Cannot resume while " + getName());
}
    
void InstanceState::complete(){
    throw std::logic_error("Cannot complete while " + getName());
}
    
void InstanceState::cancel(){
    throw std::logic_error("Cannot cancel while " + getName());
}

void InstanceState::fail(const Issue& issue){
    throw std::logic_error("Cannot fail while " + getName());
}

bool InstanceState::canCancel() const {
    return false;
}

//CreatedState  
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

//RunningState
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
    i->setState(new FailedState(*i));
    i->publish("InstanceCancelled");
} 
    
void RunningState::fail(const Issue& issue) {
    WorkflowInstance* i = instance;
    i->setState(new FailedState(*i));
    i->publish("InstanceFailed");
}
    
bool RunningState::canCancel() const {
    return true;
}
    
std::string RunningState::getName() const {
    return "Running";
}

//SuspendedState
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

//CompletedState
CompletedState::CompletedState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string CompletedState::getName() const {
    return "Completed";
} 

//CancelledState
CancelledState::CancelledState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string CancelledState::getName() const {
    return "Cancelled";
} 

//FailedState
FailedState::FailedState(WorkflowInstance& i) : InstanceState(i) {}
    
std::string FailedState::getName() const {
    return "Failed";
}  