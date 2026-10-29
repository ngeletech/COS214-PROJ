#include "WorkflowCommand.h"
#include "InstanceSnapshot.h"

CommandInvoker::CommandInvoker(RollbackManager* rollback) : rollback(rollback) {}
void CommandInvoker::submit(WorkflowCommand* command) {
    queue.push(command);
}
CommandInvoker::~CommandInvoker() {
    while (!queue.empty()) {
        delete queue.front();
        queue.pop();
    }
    while (!history.empty()) {
        delete history.back();
        history.pop_back();
    }
}
void CommandInvoker::runNext() {
    if (!queue.empty()) {
        WorkflowCommand* command = queue.front();
        queue.pop();
        command->execute();
        history.push_back(command);
    }
}

void CommandInvoker::undoLast() {
    if (history.empty()) {
        std::cout << "No commands to undo." << std::endl;
        return;
    }

    WorkflowCommand* command = history.back();
    history.pop_back();

    WorkflowInstance* target = command->getTarget();

    if (target == nullptr) {
        std::cout << "Cannot undo a command without a target instance." << std::endl;
    } else if (rollback != nullptr && rollback->canRollback(target->getId())) {
        rollback->rollback(target);
    } else {
        std::cout << "No snapshot available for rollback." << std::endl;
    }
    delete command;
}
