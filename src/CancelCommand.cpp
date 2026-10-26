#include "WorkflowCommand.h"
#include "WorkflowInstance.h"

CancelInstanceCommand::CancelInstanceCommand(WorkflowInstance* target) : WorkflowCommand(target) {}
void CancelInstanceCommand::execute() {
    target->cancel();
}
