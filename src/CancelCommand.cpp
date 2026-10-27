#include "WorkflowCommand.h"

CancelInstanceCommand::CancelInstanceCommand(WorkflowInstance* target) : WorkflowCommand(target) {}
void CancelInstanceCommand::execute() {
    target->cancel();
}
