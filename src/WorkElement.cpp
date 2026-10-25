#include "../include/composite.h"

#include <string>

WorkElement::WorkElement(const std::string& name){
    this->name = name;
    this->status = WorkItemStatus::CREATED;
    instance = nullptr;
    parent = nullptr;
}

WorkElement::~WorkElement(){}

std::string WorkElement::getName() const {
    return name;
}

WorkItemStatus WorkElement::getStatus() const {
    return status;
} 

WorkElement::getInstance(WorkFlowInstance* instance) const {
    return instance;
}

void WorkElement::setStatus(WorkItemStatus status) {
    this->status = WorkItemStatus::status;
}

void WorkElement::setInstance(WorkflowInstance* instance){
    this->instance = instance;
}

void WorkElement::setParent(WorkGroup* parent){
    this->parent = parent;
}

// no op, I typecast it to void to supress warnings
void WorkElement::assignTo(Participant* participant){
    void(participant);
}

WorkGroup* WorkElement::asGroup(){
    return nullptr;
}

Participant* WorkElement::getAssignee() const {
    return nullptr;
}

bool WorkElement::requiresAssignee() const {
    return false;
}

int WorkElement::getPriority() const{
    return 0;
}