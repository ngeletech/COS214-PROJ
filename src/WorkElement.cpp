#include "../include/composite.h"
#include "../include/WorkData.h"
#include "../include/WorkflowInstance.h"


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

WorkflowInstance* WorkElement::getInstance() const {
    return instance;
}

void WorkElement::setStatus(WorkItemStatus status) {
    this->status = status;
}

void WorkElement::setInstance(WorkflowInstance* instance){
    this->instance = instance;
}

void WorkElement::setParent(WorkGroup* parent){
    this->parent = parent;
}

// no op, I typecast it to void to supress warnings
void WorkElement::assignTo(Participant* participant){
    (void)participant;
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

bool WorkElement::isAvailable() const {
    if(this->getStatus() != WorkItemStatus::CREATED) return false;


    const WorkElement* current = this;
    bool canStartChild = false;
    while(current->parent != nullptr){
        canStartChild = current->parent->canStartChild(current->getName());
        if(canStartChild){
            current = current->parent;
        }else{
            return false;
        }
    }


    return true;
}


WorkData WorkElement::getPayload() const {
    if(instance == nullptr){
        return WorkData();
    }else{
        return instance->getData();;
    }
}