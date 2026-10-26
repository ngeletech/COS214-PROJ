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