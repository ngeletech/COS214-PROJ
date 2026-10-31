#include "../include/composite.h"
#include "../include/Participant.h"
#include "../include/State.h"
#include "../include/WorkflowEngine.h"
#include "../include/Issue.h"

#include <iostream>
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
        getInstance()->publish("TASK_COMPLETED", getName());
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
    if (!requiresAssignee()) {
        complete();
    }
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

        instance->publish("ElementExecuted" , getName());

    } else {
        std::cout << "[Task]: Task '" << getName() << "' has been executed, but no WorkflowInstance is associated." << std::endl;
    }
}

void Task::reportIssue(Issue issue) {
    WorkflowInstance* inst = getInstance();
    if (inst != nullptr && inst->getEngine() != nullptr) {
        inst->getEngine()->handleIssue(issue, *inst);
    }
}