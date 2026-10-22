#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>

using namespace std;

// Forward declaration
class WorkflowEvent;
class WorkflowEngine;
class AuditHistory;

// ===============================
// WorkflowListener
// ===============================
class WorkflowListener {
public:
    virtual void update(const WorkflowEvent& event) = 0;
    virtual ~WorkflowListener() {}
};

// ===============================
// WorkflowEventPublisher
// ===============================
class WorkflowEventPublisher {
private:
    vector<WorkflowListener*> listeners;

public:
    void subscribe(WorkflowListener* listener);
    void publish(const string& event);
    ~WorkflowEventPublisher();
};

// ===============================
// AuditLogger
// ===============================
class AuditLogger : public WorkflowListener {
private:
    AuditHistory* history;

public:
    AuditLogger(AuditHistory* history);
    void update(const WorkflowEvent& event) override;
};

// ===============================
// NextItemActivator
// ===============================
class NextItemActivator : public WorkflowListener {
private:
    WorkflowEngine* engine;

public:
    NextItemActivator(WorkflowEngine* engine);
    void update(const WorkflowEvent& event) override;
};

// ===============================
// NotificationListener
// ===============================
class NotificationListener : public WorkflowListener {
private:
    string eventType;
    vector<string> recipients;
    string message;

public:
    NotificationListener(
        string type,
        vector<string> recipients,
        string message
    );

    void update(const WorkflowEvent& event) override;
};

// ===============================
// DashboardListener
// ===============================
class DashboardListener : public WorkflowListener {
private:
    map<int, string> statusView;

public:
    void update(const WorkflowEvent& event) override;
    string getStatus(int instanceId) const;
};

#endif