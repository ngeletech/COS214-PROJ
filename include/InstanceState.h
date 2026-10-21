#ifndef INSTANCESTATE_H
#define INSTANCESTATE_H

#include <string>
class WorkflowInstance;
class Issue;

class InstanceState {
protected:
    WorkflowInstance* instance;
public:
    InstanceState();
    virtual void handle(const std::string& request) = 0;
    virtual bool canCancel() const = 0;
    void start();
    void suspend();
    void resume();
    void complete();
    void cancel();
    void fail(const Issue& issue);
    std::string getName();
};

class CreatedState : public InstanceState {
public:
    void handle(const std::string& request) override;
    bool canCancel() const override; 
};

class RunningState : public InstanceState {
public:
    void handle(const std::string& request) override;
    bool canCancel() const override;  
};

class SuspendedState : public InstanceState {
public:
    void handle(const std::string& request) override;
    bool canCancel() const override;  
};

class CompletedState : public InstanceState {
public:
    void handle(const std::string& request) override;
    bool canCancel() const override;  
};

class CancelledState : public InstanceState {
public:
    void handle(const std::string& request) override;
    bool canCancel() const override;  
};

class FailedState : public InstanceState {
public:
    void handle(const std::string& request) override;
    bool canCancel() const override;  
};

#endif
