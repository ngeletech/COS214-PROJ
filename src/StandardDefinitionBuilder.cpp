//Concrete Builder
#include "../include/Builder.h"

StandardDefinitionBuilder::StandardDefinitionBuilder() {
    this->reset();


}

StandardDefinitionBuilder::~StandardDefinitionBuilder() {
    if (this->product != nullptr) {
        delete this->product;
    }


}

void StandardDefinitionBuilder::setName(const std::string& name) {
    if (this->product != nullptr) {
        this->product->name = name;
    }

}

void StandardDefinitionBuilder::addWorkItem(const WorkItemSpec& spec) {
    if (this->product != nullptr) {
        this->product->workItemSpecs.push_back(spec);
    }

}

void StandardDefinitionBuilder::setAssignmentRule(AssignmentStrategy* strategy) {
     if (this->product != nullptr) {
        this->product->assignmentStrategy = strategy;
    }

}

void StandardDefinitionBuilder::setEscalationChain(EscalationHandler* handler) {
    if (this->product != nullptr) {
        this->product->escalationHandler = handler;
    }

}

WorkflowDefinition* StandardDefinitionBuilder::build() {
    if (this->product == nullptr) {
        throw std::runtime_error("StandardDefinitionBuilder: No active product to build!");
    }

    if (this->product->workItemSpecs.empty()) {
        throw std::runtime_error("StandardDefinitionBuilder: Cannot build; empty workflow with no task!");
    }

    WorkflowDefinition* finishedProduct = this->product;

    this->product = nullptr;
    this->reset();

    return finishedProduct;

}

void StandardDefinitionBuilder::reset() {
    this->product = new WorkflowDefinition();
}
