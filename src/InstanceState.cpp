#include "InstanceState.h"

//InstanceState
InstanceState::InstanceState(WorkflowInstance& i) : instance(&i) {

}
        
void InstanceState::start(){

}
    
void InstanceState::suspend(){

} 
    
void InstanceState::resume(){

}
    
void InstanceState::complete(){

}
    
void InstanceState::cancel(){

}

void InstanceState::fail(const Issue& issue){

}

bool InstanceState::canCancel() const {

}
    
std::string InstanceState::getName() const {

}

//CreatedState  
CreatedState::CreatedState(WorkflowInstance& i) : InstanceState(i) {

}
    
void CreatedState::start() {

}
    
void CreatedState::cancel() {

}
    
bool CreatedState::canCancel() const {

}
    
std::string CreatedState::getName() const {

}

//RunningState
RunningState::RunningState(WorkflowInstance& i) : InstanceState(i) {

}
    
void RunningState::suspend() {

}
    
void RunningState::complete() {

}
    
void RunningState::cancel() {

} 
    
void RunningState::fail(const Issue& issue) {

}
    
bool RunningState::canCancel() const {

}
    
std::string RunningState::getName() const {

}

//SuspendedState
SuspendedState::SuspendedState(WorkflowInstance& i) : InstanceState(i) {

}
    
void SuspendedState::resume() {

}
    
void SuspendedState::cancel() {

}
    
bool SuspendedState::canCancel() const {

}
    
std::string SuspendedState::getName() const {

} 

//CompletedState
CompletedState::CompletedState(WorkflowInstance& i) : InstanceState(i) {

}
    
std::string CompletedState::getName() const {

} 

//CancelledState
CancelledState::CancelledState(WorkflowInstance& i) : InstanceState(i) {

}
    
std::string CancelledState::getName() const {

} 

//FailedState
FailedState::FailedState(WorkflowInstance& i) : InstanceState(i) {

}
    
std::string FailedState::getName() const {

}  