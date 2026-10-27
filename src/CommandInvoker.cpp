#include "WorkflowCommand.h"
#include "RollbackManager.h"

CommandInvoker::CommandInvoker(RollbackManager* rollback) : rollback(rollback) {}
void CommandInvoker::submit(WorkflowCommand* command) {
    commandQueue.push(command);
}
CommandInvoker::~CommandInvoker() {
    while (!commandQueue.empty()) {
        delete commandQueue.front();
        commandQueue.pop();
    }
    while (!commandHistory.empty()) {
        delete commandHistory.back();
        commandHistory.pop_back();
    }
}
void CommandInvoker::runNext() {
    if (!commandQueue.empty()) {
        WorkflowCommand* command = commandQueue.front();
        commandQueue.pop();
        command->execute();
        commandHistory.push_back(command);
    }
}
void CommandInvoker::undoLast() {
    if (!commandHistory.empty()) {
        WorkflowCommand* command = commandHistory.back();
        commandHistory.pop_back();
        rollback->rollback(command);
        delete command;
    }
}