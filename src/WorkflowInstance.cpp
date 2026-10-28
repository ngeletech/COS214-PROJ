#include "State.h"
#include "WorkItemStatus.h"
#include "InstanceSnapshot.h"
#include "Iterator.h"
#include "composite.h"
#include "Participant.h"
#include "WorkflowListener.h"
#include "WorkflowEngine.h"
#include <map>

WorkflowInstance::WorkflowInstance(int id, const WorkflowDefinition* definition, WorkData data, WorkflowEngine* engine)
    : id(id), data(data), state(nullptr), definition(definition), root(nullptr), engine(engine) {
    state = new CreatedState(*this);
}
    
WorkflowInstance::~WorkflowInstance() {
    delete state;
    delete root;
}
    
int WorkflowInstance::getId() const {
    return id;
}
    
const WorkflowDefinition* WorkflowInstance::getDefinition() const{
    return definition;
}
   
void WorkflowInstance::setRoot(WorkGroup* newRoot) {
    if (newRoot == root) return;
    delete root;
    root = newRoot;
}

WorkGroup* WorkflowInstance::getRoot() const {
    return root;
}
    
std::string WorkflowInstance::getStateName() const {
    return state->getName();
}
    
const WorkData& WorkflowInstance::getData() const {
    return data;
}

void WorkflowInstance::setData(const std::string& key, const std::string& value) {
    data.addData(key, value);
    publish("DataChanged");
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
    if (listener != nullptr) {
        listeners.push_back(listener);
    }
}
    
void WorkflowInstance::publish(const std::string& event) {
    for (std::size_t k = 0; k < listeners.size(); ++k) {
        listeners[k]->update(event);
    }
}

//memento
InstanceSnapshot WorkflowInstance::saveSnapshot() {
    InstanceSnapshot snap;
    snap.data = data;
    snap.stateName = state->getName();

    for (DepthFirstIterator it(root); !it.isDone(); it.next()) {
        WorkElement* element = it.current();
        snap.elementStatuses[element->getName()] = element->getStatus();

        Participant* who = element->getAssignee();
        if (who != nullptr) {
            snap.assignments[element->getName()] = who->getId();
        }
    }
    return snap;
}
    
void WorkflowInstance::restore(const InstanceSnapshot& snap) {
    data = snap.data;

    for (DepthFirstIterator it(root); !it.isDone(); it.next()) {
        WorkElement* element = it.current();

        std::map<std::string, int>::const_iterator a = snap.assignments.find(element->getName());
        if (a != snap.assignments.end() && engine != nullptr) {
            Participant* who = engine->findParticipant(a->second);
            if (who != nullptr) {
                element->assignTo(who);
            }
        }

        // set the status last, because assignTo may change it
        std::map<std::string, WorkItemStatus>::const_iterator s = snap.elementStatuses.find(element->getName());
        if (s != snap.elementStatuses.end()) {
            element->setStatus(s->second);
        }
    }

    setState(InstanceState::create(snap.stateName, *this));
}

