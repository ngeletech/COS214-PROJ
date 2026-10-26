#include "../include/composite.h"

#include <string>

Task::Task(const std::string& name) : WorkElement(name) {
    assignee = nullptr;
}

bool Task::isComplete() const {
    return getStatus() == WorkItemStatus::COMPLETED;
}

// This will be completed once the dependencies are met 
void Task::complete(){

    setStatus(WorkItemStatus::COMPLETED);


    if(parent != nullptr){
        parent->complete();
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

