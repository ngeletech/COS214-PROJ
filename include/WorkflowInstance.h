#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <string>
#include "WorkData.h"
class WorkflowDefinition;
class InstanceState;
class Issue;
class WorkGroup;
class WorkflowEngine;
class WorkflowListener;
class InstanceSnapshot;

class WorkflowInstance {
private:
    int id;
    WorkData data;
    InstanceState* state;
    const WorkflowDefinition* definition;
    WorkGroup* root;
    WorkflowEngine* engine;
public:
    WorkflowInstance(int id, const WorkflowDefinition*, WorkflowEngine* engine, WorkData data);
    ~WorkflowInstance();

    int getId() const;
    const WorkflowDefinition* getDefinition() const;
    WorkGroup* getRoot() const;
    std::string getStateName() const;

    void start();
    void suspend();
    void resume();
    void complete();
    void cancel();
    void fail(const Issue& issue);

    void setState(InstanceState* newState);

    void subscribe(WorkflowListener* listener);
    void publish(const std::string& event);

    InstanceSnapshot saveSnapshot();
    void restore(const InstanceSnapshot& snapshot);

    const WorkData& getData() const;
    void setData(const std::string& key, const std::string& value);
};

#endif
