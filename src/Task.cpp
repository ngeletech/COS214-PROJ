#include "../include/composite.h"
#include "../include/State.h"

#include <string>
#include <iostream>

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
    if (!checkReady()) {
        std::cout << "[Task]: Task '" << getName() << "' is not ready to execute." << std::endl;
        return;
    }

    setStatus(WorkItemStatus::STARTED);
    std::cout << "[Task]: Executing task '" << getName() << "'." << std::endl;

    doWork();

    Task::notifyListeners();

    complete();
}

bool Task::checkReady() const {
    WorkItemStatus status = getStatus();

    if (requiresAssignee()) {
        return (status == WorkItemStatus::ASSIGNED) && (assignee != nullptr);
    }

    return (status == WorkItemStatus::AVAILABLE || status == WorkItemStatus::CREATED);
}

void Task::notifyListeners(){
    WorkflowInstance* instance = getInstance();
    
    if (instance != nullptr) {
        std::cout << "[Task]: Task '" << getName() << "' has been executed. Notifying listeners." << std::endl;

        instance->publish("ElementExecuted: " + getName());

    } else {
        std::cout << "[Task]: Task '" << getName() << "' has been executed, but no WorkflowInstance is associated." << std::endl;
    }
}

