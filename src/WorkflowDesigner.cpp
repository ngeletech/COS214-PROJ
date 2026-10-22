//Director
#include "WorkflowDesigner.h"

WorkflowDesigner::WorkflowDesigner(WorkflowDefinitionBuilder* builder) {
    this->builder = builder;

}

WorkflowDefinition* WorkflowDesigner::construct(const std::string& name, const std::vector<WorkItemSpec>& specs, AssignmentStrategy* strategy, EscalationHandler* chain) {

    
}
