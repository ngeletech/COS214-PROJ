#ifndef TIVIDYFACADE_H
#define TIVIDYFACADE_H

#include <string>
#include <vector>
#include "WorkData.h"
#include "Builder.h"

class WorkflowDefinition;
class AssignmentStrategy;
class EscalationHandler;
class Participant;
class WorkflowListener;
class WorkflowCommand;
class WorkflowInstance;
class AuditHistory;
class RollbackManager;
class CommandInvoker;
class AssignmentService;
class WorkItemFactoryRegistry;
class WorkflowEngine;
class WorkflowDesigner;
class AuditLogger;
class NextItemActivator;

/**
 * @class TividyFacade
 * @brief The single entry point for clients (the TUI). Creates and wires the engine and its
 *        helpers, and hides them behind a few simple operations.
 *
 * Owns everything it creates, plus the participants and listeners passed to it.
 * Invalid requests throw (std::invalid_argument for unknown ids or names,
 * std::logic_error for actions the instance's state refuses), and the caller catches them.
 */
class TividyFacade {
private:
    AuditHistory* audit;
    RollbackManager* rollback;
    CommandInvoker* invoker;
    AssignmentService* assignment;
    AssignmentStrategy* defaultStrategy; 
    WorkItemFactoryRegistry* factories;
    WorkflowEngine* engine;
    WorkflowDesigner* designer;
    AuditLogger* auditLogger;
    NextItemActivator* activator;
    std::vector<Participant*> participants;
    std::vector<WorkflowListener*> listeners;
    std::vector<std::string> definitionNames;

    TividyFacade(const TividyFacade&);                 // no copying
    TividyFacade& operator=(const TividyFacade&);

    WorkflowInstance& requireInstance(int instanceId) const;
    void run(WorkflowCommand* command);                // submit, then run it

public:
    TividyFacade();
    ~TividyFacade();

    WorkflowDefinition* defineWorkflow(const std::string& name,
                                       const std::vector<WorkItemSpec>& specs,
                                       AssignmentStrategy* strategy,
                                       EscalationHandler* chain);
    std::vector<std::string> listDefinitions() const;

    int  startWorkflow(const std::string& defName, const WorkData& data);
    void cancelWorkflow(int instanceId);
    void changeData(int instanceId, const std::string& key, const std::string& value);
    void reassignItem(int instanceId, const std::string& elementName, Participant* p);
    void completeItem(int instanceId, const std::string& elementName);
    void undoLast();

    std::string trackStatus(int instanceId) const;
    std::vector<std::string> getAvailableWork(int instanceId) const;
    std::string generateReport(int instanceId) const;

    void registerParticipant(Participant* participant);
    void addListener(WorkflowListener* listener);
};

#endif