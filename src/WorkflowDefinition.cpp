//Product
#include "../include/Builder.h"


WorkflowDefinition::WorkflowDefinition() : name(""), version(0), workItemSpecs(), assignmentStrategy(nullptr), escalationHandler(nullptr) {

}

void WorkflowDefinition::setVersion(int version) {
    this->version = version;
}

std::string WorkflowDefinition::getName() const {
    return this->name;

}

int WorkflowDefinition::getVersion() const {
    return this->version;

}

const std::vector<WorkItemSpec>& WorkflowDefinition::getWorkItemSpecs() const {
    return this->workItemSpecs;

}

AssignmentStrategy* WorkflowDefinition::getAssignmentStrategy() const {
    return this->assignmentStrategy;

}

EscalationHandler* WorkflowDefinition::getEscalationHandler() const {
    return this->escalationHandler;
}

WorkflowDefinition::~WorkflowDefinition() {
    delete assignmentStrategy;
    delete escalationHandler;

}
