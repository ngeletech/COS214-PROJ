#include "WorkflowCommand.h"
#include "WorkflowEngine.h"

// WorkflowCommand
WorkflowCommand::WorkflowCommand(WorkflowInstance* target) : target(target) {}
WorkflowCommand::~WorkflowCommand() {}
WorkflowInstance* WorkflowCommand::getTarget() {
    return target;
}   

