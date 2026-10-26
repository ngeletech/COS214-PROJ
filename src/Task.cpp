#include "../include/composite.h"
#include "../include/Participant.h"
#include "../include/WorkflowInstance.h"

#include <string>

Task::Task(const std::string& name) : WorkElement(name) {
    assignee = nullptr;
}

bool Task::isComplete() const {
    return getStatus() == WorkItemStatus::COMPLETED;
}

// marks the task finished it then tells the assignee its group and the instance
void Task::complete(){
    setStatus(WorkItemStatus::COMPLETED);

    if(assignee != nullptr){
        assignee->decrementWorkload();
    }

    // lets the group check whether all of its items are done
    if(parent != nullptr){
        parent->complete();
    }

    // listeners react to this event
    if(getInstance() != nullptr){
        getInstance()->publish("ElementCompleted");
    }
}

// this func only stores who is doing the task, the engine changes the status
void Task::assignTo(Participant* participant){
    assignee = participant;
}

Participant* Task::getAssignee() const {
    return assignee;
}

bool Task::requiresAssignee() const {
    return true;
}

// Template Method functions below

void Task::execute(){
}

bool Task::checkReady() const {
    return false;
}

void Task::notifyListeners(){
}

