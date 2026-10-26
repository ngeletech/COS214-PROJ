#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>

using namespace std;

// Forward declaration
class WorkflowEvent;
class WorkflowEngine;
class AuditHistory;

/**
 * @brief Abstract observer interface for workflow event notifications.
 *
 * All classes interested in receiving workflow events must inherit
 * from WorkflowListener and implement the update() operation.
 */
class WorkflowListener {
public:
    /**
     * @brief Receives and processes a workflow event.
     *
     * @param event The workflow event being published.
     */
    virtual void update(const WorkflowEvent& event) = 0;
    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkflowListener() {}
};

/**
 * @brief Publishes workflow events to registered listeners.
 *
 * Implements the Subject role in the Observer pattern.
 * Maintains a collection of observers and notifies them
 * whenever an event occurs.
 */
class WorkflowEventPublisher {
private:
    /**
     * @brief Collection of subscribed listeners.
     */
    vector<WorkflowListener*> listeners;

public:
    /**
     * @brief Registers a listener for event notifications.
     *
     * @param listener Observer to subscribe.
     */
    void subscribe(WorkflowListener* listener);
    /**
     * @brief Publishes an event to all subscribed listeners.
     *
     * @param event Event information to be distributed.
     */
    void publish(const string& event);
    /**
     * @brief Destructor.
     */
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