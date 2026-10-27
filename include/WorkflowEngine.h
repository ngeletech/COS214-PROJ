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
class AssignmnentService;
class Participant;
class WorkflowListener;
class Issue;
class WorkflowElement;

class WorkflowEngine {
private:
    std::map<std::string, std::vector<WorkflowDefinition*>> definitions;
    std::map<int, WorkflowInstance*> instances;
    EscalationHandler* escalationHandler;
    AssignmnentService* assignment;
    std::vector<WorkflowListener*> defualtListeners;
    int nextInstanceId = 1;
public:
    WorkflowEngine(AssignmnentService* assignment);
    ~WorkflowEngine();

    void registerDefinition(WorkflowDefinition* definition);
    WorkflowInstance* startInstance(std::string defName, WorkData data);
    void completeWorkItem(int instanceId, std::string elementName);
    void handleIssue(Issue& issue, WorkflowInstance& instance);
    void assignWork(WorkflowInstance& item, WorkflowElement* element);

   void activateNext(WorkflowInstance& instance);
    WorkflowInstance* getInstance(int id);

    void addParticipant(Participant* participant);
    Participant findParticipant(int id);
    void addDefaultListener(WorkflowListener* listener);
};

#endif