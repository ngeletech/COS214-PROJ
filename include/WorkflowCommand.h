#ifndef WORKFLOWCOMMAND_H
#define WORKFLOWCOMMAND_H

#include <iostream>
#include <queue>
#include <vector>
#include "WorkflowEngine.h"
#include "State.h"
#include "WorkData.h"
class RollbackManager;

class WorkflowCommand {
    protected:
        WorkflowInstance* target;
    public:
        WorkflowCommand(WorkflowInstance* target);
        virtual ~WorkflowCommand();
        virtual void execute() = 0;
        WorkflowInstance* getTarget();
};

class StartInstanceCommand : public WorkflowCommand {
    private:
        WorkflowEngine* engine;
        std::string defName;
        WorkData data;
    public:
        StartInstanceCommand(WorkflowEngine* engine, std::string defName, WorkData data);
        void execute() override;
};

class CancelInstanceCommand : public WorkflowCommand {
    public:
        CancelInstanceCommand(WorkflowInstance* target);
        void execute() override;
};

class ChangeDataCommand : public WorkflowCommand {
    private:
        std::string key;
        std::string value;
    public:
        ChangeDataCommand(WorkflowInstance* target, std::string key, std::string value);
        void execute() override;
};

class ReassignItemCommand : public WorkflowCommand {
    private:
        std::string elementName;
        Participant* newAssignee;
    public:
        ReassignItemCommand(WorkflowInstance* target, std::string elementName, Participant* newAssignee);
        void execute() override;
};

class CommandInvoker {
    private:
        std::queue<WorkflowCommand*> queue;
        std::vector<WorkflowCommand*> history;
        RollbackManager* rollback;
    public:
        CommandInvoker(RollbackManager* rollback);
        ~CommandInvoker();
        void submit(WorkflowCommand* command);
        void runNext();
        void undoLast();
};



#endif /* WORKFLOWCOMMAND_H */