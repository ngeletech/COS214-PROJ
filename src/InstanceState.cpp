#include "InstanceState.h"

//InstanceState
void InstanceState::handle(const std::string& request){

}

bool InstanceState::canCancel() const {

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
    
std::string InstanceState::getName(){

}

//CreatedState  
void CreatedState::handle(const std::string& request){

}
    
bool CreatedState::canCancel() const {

}

//RunningState
void RunningState::handle(const std::string& request){

}
    
bool RunningState::canCancel() const {

}

//SuspendedState
void SuspendedState::handle(const std::string& request){

}
    
bool SuspendedState::canCancel() const {

} 

//CompletedState
void CompletedState::handle(const std::string& request){

}
    
bool CompletedState::canCancel() const {

}

//CancelledState
void CancelledState::handle(const std::string& request){

}
    
bool CancelledState::canCancel() const {

}

//FailedState
void FailedState::handle(const std::string& request){

}

bool FailedState::canCancel() const {

}