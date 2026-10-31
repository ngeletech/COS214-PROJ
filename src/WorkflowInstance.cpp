#include "State.h"
#include "composite.h"
#include "InstanceSnapshot.h"
#include "Iterator.h"
#include "Participant.h"
#include "Observer.h"
#include "WorkflowEvent.h"
#include "WorkflowEngine.h"
#include <map>

static bool holdsWork(WorkElement* element) {
    WorkItemStatus s = element->getStatus();
    return element->getAssignee() != nullptr && (s == WorkItemStatus::ASSIGNED || s == WorkItemStatus::STARTED);
}

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

WorkflowEngine* WorkflowInstance::getEngine() const {
    return engine;
}

void WorkflowInstance::setData(const std::string& key, const std::string& value) {
    data.addData(key, value);
    publish("DataChanged");
}

//work tree helpers
WorkElement* WorkflowInstance::findElement(const std::string& name) {
    if (root == nullptr) return nullptr;

    for (DepthFirstIterator it(root); !it.isDone(); it.next()) {
        WorkElement* element = it.current();
        if (element != root && element->getName() == name) {
            return element;
        }
    }
    return nullptr;
}

void WorkflowInstance::cancelOutstandingWork() {
    if (root == nullptr) return;

    for (DepthFirstIterator it(root); !it.isDone(); it.next()) {
        WorkElement* element = it.current();
        WorkItemStatus s = element->getStatus();
        if(holdsWork(element)) element->getAssignee()->decrementWorkload();

        if (s != WorkItemStatus::COMPLETED &&
            s != WorkItemStatus::REJECTED &&
            s != WorkItemStatus::CANCELLED) {
            element->setStatus(WorkItemStatus::CANCELLED);
        }
    }
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
    
void WorkflowInstance::publish(const std::string& event, const std::string& elementName) {
    /*WorkflowEvent ev(event, id, "", "");
    for (std::size_t k = 0; k < listeners.size(); ++k) {
        listeners[k]->update(ev);
    }*/
    pending.push_back(WorkflowEvent(event, id, elementName, ""));
    if (publishing) return; // a listener is still running; its events go out after it
    publishing = true;
    for(std::size_t e = 0; e < pending.size(); ++e) {
        WorkflowEvent ev = pending[e];
        for (std::size_t k = 0; k < listeners.size(); ++k) {
            listeners[k]->update(ev);
        }   
    }
    pending.clear();
    publishing = false;

    
}

//memento
InstanceSnapshot WorkflowInstance::saveSnapshot() {
    InstanceSnapshot snap;
    snap.data = data;
    snap.stateName = state->getName();

    if (root != nullptr) {
        for (DepthFirstIterator it(root); !it.isDone(); it.next()) {
            WorkElement* element = it.current();
            snap.elementStatuses[element->getName()] = element->getStatus();

            Participant* who = element->getAssignee();
            if (who != nullptr) {
                snap.assignments[element->getName()] = who->getId();
            }
        }
    }
    return snap;
}
    
void WorkflowInstance::restore(const InstanceSnapshot& snap) {
    data = snap.data;

    if (root != nullptr) {
        for (DepthFirstIterator it(root); !it.isDone(); it.next()) {
            WorkElement* element = it.current();

            if(holdsWork(element)) element->getAssignee()->decrementWorkload();
            if (engine != nullptr) {
                Participant* who = nullptr;
                std::map<std::string, int>::const_iterator a = snap.assignments.find(element->getName());
                if (a != snap.assignments.end()) who = engine->findParticipant(a->second);
                element->assignTo(who);
            }

            // set the status last, because assignTo may change it
            std::map<std::string, WorkItemStatus>::const_iterator s = snap.elementStatuses.find(element->getName());
            if (s != snap.elementStatuses.end()) {
                element->setStatus(s->second);
            }

            if(holdsWork(element)) element->getAssignee()->incrementWorkload();
        }
    }

    setState(InstanceState::create(snap.stateName, *this));
}