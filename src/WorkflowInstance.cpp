#include "WorkflowInstance.h"
#include "InstanceState.h"
// #include "WorkGroup.h"
// #include "WorkflowListener.h"
// #include "WorkflowEvent.h"
// #include "InstanceSnapshot.h"

WorkflowInstance::WorkflowInstance(int id, const WorkflowDefinition*, WorkflowEngine* engine, WorkData data) {

}
    
WorkflowInstance::~WorkflowInstance() {

}

    
int WorkflowInstance::getId() const {
    return id;
}
    
// const WorkflowInstance::WorkflowDefinition* getDefinition() const{
//     return definition;
// }
    
WorkGroup* WorkflowInstance::getRoot() const {
    return root;
}
    
std::string WorkflowInstance::getStateName() const {
    return state->getName();
}
    
const WorkData& WorkflowInstance::getData() const {
    return data;
}

//state lifecycle
void WorkflowInstance::start() {
    state->start();
}
    
void WorkflowInstance::suspend() {
    state->suspend();
}
    
void WorkflowInstance::resume() {
    state->resume();
}
    
void WorkflowInstance::complete() {
    state->complete();
}
    
void WorkflowInstance::cancel() {
    state->cancel();
}
    
void WorkflowInstance::fail(const Issue& issue) {
    state->fail(issue);
}
    
void WorkflowInstance::setState(InstanceState* newState) {
    InstanceState* old = state;
    state = newState;
    delete old;
}

//observer
void WorkflowInstance::subscribe(WorkflowListener* listener) {
    listeners.push_back(listener);
}
    
void WorkflowInstance::publish(const std::string& event) {

}

// InstanceSnapshot WorkflowInstance::saveSnapshot() {


// }
    
void WorkflowInstance::restore(const InstanceSnapshot& snapshot) {

}

    
void WorkflowInstance::setData(const std::string& key, const std::string& value) {
    data.addData(key, value);
    publish("DataChanged");
}
