//Director
#include "../include/Builder.h"

WorkflowDesigner::WorkflowDesigner(WorkflowDefinitionBuilder* builder) {
    this->builder = builder;

}

WorkflowDefinition* WorkflowDesigner::construct(const std::string& name, const std::vector<WorkItemSpec>& specs, AssignmentStrategy* strategy, EscalationHandler* chain) {

    if (this->builder == nullptr) {
        throw std::runtime_error("WorkflowDesigner: No builder attached to direct construction");

    }

    //starting fresh builder
    this->builder->reset();

    //setName
    this->builder->setName(name);

    //add specifications
    for (const auto& spec : specs) {
        this->builder->addWorkItem(spec);
    }

    //attach strategy and erroe escalation rules
    this->builder->setAssignmentRule(strategy);
    this->builder->setEscalationChain(chain);
    
    //return completed blueprint
    return this->builder->build();

}

WorkflowDesigner::~WorkflowDesigner() {
    delete builder;
}