#include "../include/Decorator.h"
#include "../include/WorkData.h"
#include <string>
using namespace std;

WorkElementDecorator::WorkElementDecorator(WorkElement* element) : WorkElement(element->getName()) {
    this->wrapped = element;
}

WorkElementDecorator::~WorkElementDecorator() {
    if (!wrapped) return;
    delete wrapped; 
}

void WorkElementDecorator::execute() {
    if(!wrapped) return;
    wrapped->execute();
}

void WorkElementDecorator::complete() {
    if(!wrapped) return;
    wrapped->complete();
}

bool WorkElementDecorator::isComplete() const {
    if(!wrapped) return false;
    return wrapped->isComplete();
}

bool WorkElementDecorator::isAvailable() {
    if(!wrapped) return false;
    return wrapped->isAvailable();
}

WorkData WorkElementDecorator::getPayload() const {
    return wrapped->getPayload();
}

string WorkElementDecorator::getName() {
    if(!wrapped) return "";
    return wrapped->getName();
}

WorkItemStatus WorkElementDecorator::getStatus() {
    if(!wrapped) return WorkItemStatus::CREATED;
    return wrapped->getStatus();
}

void WorkElementDecorator::setStatus(WorkItemStatus status) {
    if(!wrapped) return;
    wrapped->setStatus(status);
}

WorkflowInstance* WorkElementDecorator::getInstance() {
    if(!wrapped) return nullptr;
    return wrapped->getInstance();
}

void WorkElementDecorator::setInstance(WorkflowInstance* instance) {
    if(!wrapped) return;
    wrapped->setInstance(instance);
}

void WorkElementDecorator::assignTo(Participant* participant) {
    if(!wrapped) return;
    wrapped->assignTo(participant);
}