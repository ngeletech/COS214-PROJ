#ifndef INSTANCESTATE_H
#define INSTANCESTATE_H

#include <string>
class WorkflowInstance;
class Issue;

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

#endif
