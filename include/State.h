#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>
#include "WorkData.h"

class WorkflowDefinition;
class Issue;
class WorkGroup;
class WorkflowEngine;
class WorkflowListener;
class InstanceSnapshot;
class WorkflowInstance;

// ---------------- State pattern ----------------

class InstanceState {
protected:
    WorkflowInstance* instance;
public:
    InstanceState(WorkflowInstance& i);
    virtual ~InstanceState() {}
    virtual void start();
    virtual void suspend();
    virtual void resume();
    virtual void complete();
    virtual void cancel();
    virtual void fail(const Issue& issue);

    virtual bool canCancel() const;
    virtual std::string getName() const = 0;
    static InstanceState* create(const std::string& name, WorkflowInstance& inst);
};

class CreatedState : public InstanceState {
public:
    CreatedState(WorkflowInstance& i);
    void start() override;
    void cancel() override;
    bool canCancel() const override;
    std::string getName() const override;
};

class RunningState : public InstanceState {
public:
    RunningState(WorkflowInstance& i);
    void suspend() override;
    void complete() override;
    void cancel() override;
    void fail(const Issue& issue) override;
    bool canCancel() const override;
    std::string getName() const override;
};

class SuspendedState : public InstanceState {
public:
    SuspendedState(WorkflowInstance& i);
    void resume() override;
    void cancel() override;
    bool canCancel() const override;
    std::string getName() const override;
};

class CompletedState : public InstanceState {
public:
    CompletedState(WorkflowInstance& i);
    std::string getName() const override;
};

class CancelledState : public InstanceState {
public:
    CancelledState(WorkflowInstance& i);
    std::string getName() const override;
};

class FailedState : public InstanceState {
public:
    FailedState(WorkflowInstance& i);
    std::string getName() const override;
};

// ---------------- Context ----------------

class WorkflowInstance {
private:
    int id;
    WorkData data;
    InstanceState* state;
    const WorkflowDefinition* definition;
    WorkGroup* root;
    WorkflowEngine* engine;
    std::vector<WorkflowListener*> listeners;
public:
    WorkflowInstance(int id, const WorkflowDefinition*, WorkData data, WorkflowEngine* engine);
    ~WorkflowInstance();

    int getId() const;
    const WorkflowDefinition* getDefinition() const;
    void setRoot(WorkGroup* newRoot);
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



// #ifndef WORKFLOWINSTANCE_H
// #define WORKFLOWINSTANCE_H

// #include <string>
// #include<vector>
// #include "WorkData.h"
// class WorkflowDefinition;
// class InstanceState;
// class Issue;
// class WorkGroup;
// class WorkflowEngine;
// class WorkflowListener;
// class InstanceSnapshot;

// class WorkflowInstance {
// private:
//     int id;
//     WorkData data;
//     InstanceState* state;
//     const WorkflowDefinition* definition;
//     WorkGroup* root;
//     WorkflowEngine* engine;
//     std::vector<WorkflowListener*> listeners;
// public:
//     WorkflowInstance(int id, const WorkflowDefinition*, WorkData data, WorkflowEngine* engine);
//     ~WorkflowInstance();

//     int getId() const;
//     const WorkflowDefinition* getDefinition() const;
//     void setRoot(WorkGroup* newRoot);
//     WorkGroup* getRoot() const;
//     std::string getStateName() const;

//     void start();
//     void suspend();
//     void resume();
//     void complete();
//     void cancel();
//     void fail(const Issue& issue);

//     void setState(InstanceState* newState);

//     void subscribe(WorkflowListener* listener);
//     void publish(const std::string& event);

//     InstanceSnapshot saveSnapshot();
//     void restore(const InstanceSnapshot& snapshot);

//     const WorkData& getData() const;
//     void setData(const std::string& key, const std::string& value);
// };

// #endif
