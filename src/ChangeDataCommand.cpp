#include "WorkflowCommand.h"

ChangeDataCommand::ChangeDataCommand(WorkflowInstance* target, std::string key, std::string value) : WorkflowCommand(target), key(key), value(value) {}
void ChangeDataCommand::execute() {
    target->setData(key, value);
}