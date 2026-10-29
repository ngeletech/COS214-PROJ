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
    if (!history.empty()) {
        WorkflowCommand* command = history.back();
        history.pop_back();
        if (rollback->canRollback(command->getTarget()->getId())) {
            rollback->rollback(command->getTarget());
        } else {
            std::cout << "No snapshot available for rollback." << std::endl;
        }
        delete command;
    } else {
        std::cout << "No commands to undo." << std::endl;
    }
}