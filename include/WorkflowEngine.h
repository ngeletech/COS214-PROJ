/**
 * @file WorkflowEngine.h
 * @brief Core engine: registers definitions, runs instances, tracks participants.
 */
#ifndef WORKFLOWENGINE_H
#define WORKFLOWENGINE_H

#include <string>
#include <map>
#include <vector>
#include "WorkData.h"

class WorkflowDefinition;
class WorkflowInstance;
class EscalationHandler;
class WorkItemFactoryRegistry;
class AssignmentService;
class Participant;
class WorkflowListener;
class Issue;
class WorkElement;

/**
 * @class WorkflowEngine
 * @brief Creates and drives WorkflowInstance objects.
 *
 * Owns: definitions and instances (deleted in the destructor).
 * Does not own: participants, escalation handler, assignment service,
 * factory registry or default listeners.
 */
class WorkflowEngine {
private:
    std::map<std::string, std::vector<WorkflowDefinition*> > definitions; ///< Name -> versions (owned).
    std::map<int, WorkflowInstance*> instances;                           ///< Id -> instance (owned).
    EscalationHandler* escalationHandler;                                 ///< Chain head (not owned).
    WorkItemFactoryRegistry* factories;                                   ///< Not owned.
    AssignmentService* assignment;                                        ///< Not owned.
    std::vector<Participant*> participants;                               ///< Not owned.
    std::vector<WorkflowListener*> defaultListeners;                      ///< Added to every new instance.
    int nextInstanceId;

    WorkflowEngine(const WorkflowEngine&);            // no copying
    WorkflowEngine& operator=(const WorkflowEngine&);

public:
    WorkflowEngine(AssignmentService* assignment, WorkItemFactoryRegistry* factories);
    ~WorkflowEngine();

    void registerDefinition(WorkflowDefinition* definition);
    WorkflowInstance* startInstance(const std::string& defName, WorkData data);
    void completeWorkItem(int instanceId, const std::string& elementName);
    void handleIssue(const Issue& issue, WorkflowInstance& instance);
    void assignWork(WorkflowInstance& instance, WorkElement* element);
    void activateNext(WorkflowInstance& instance);
    WorkflowInstance* getInstance(int id) const;

    void addParticipant(Participant* participant);
    Participant* findParticipant(int id) const;
    void addDefaultListener(WorkflowListener* listener);
};

#endif