#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>
#include <map>

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

/**
 * @brief Records workflow events for auditing purposes.
 *
 * AuditLogger receives workflow events and stores them
 * in the system audit history.
 */
class AuditLogger : public WorkflowListener {
private:
    /**
     * @brief Audit history used to store event records.
     */
    AuditHistory* history;

public:
    /**
     * @brief Constructs an AuditLogger.
     *
     * @param history Audit history to update.
     */
    AuditLogger(AuditHistory* history);

    /**
     * @brief Records a workflow event in the audit history.
     *
     * @param event Event to record.
     */
    void update(const WorkflowEvent& event) override;
};

/**
 * @brief Activates the next workflow item when required.
 *
 * Responds to workflow events by enabling subsequent
 * tasks or workflow stages.
 */
class NextItemActivator : public WorkflowListener {
private:
    /**
     * @brief Workflow engine responsible for managing workflow progression.
     */
    WorkflowEngine* engine;

public:
    /**
     * @brief Constructs a NextItemActivator.
     *
     * @param engine Workflow engine to control.
     */
    NextItemActivator(WorkflowEngine* engine);

    /**
     * @brief Activates the next workflow item when appropriate.
     *
     * @param event Triggering workflow event.
     */
    void update(const WorkflowEvent& event) override;
};

/**
 * @brief Sends notifications when selected workflow events occur.
 *
 * NotificationListener distributes messages to recipients
 * in response to workflow events.
 */
class NotificationListener : public WorkflowListener {
private:
    /**
     * @brief Event type that triggers a notification.
     */
    string eventType;
    /**
     * @brief List of notification recipients.
     */
    vector<string> recipients;
    /**
     * @brief Notification message.
     */
    string message;

public:
    /**
     * @brief Constructs a NotificationListener.
     *
     * @param type Event type to monitor.
     * @param recipients List of recipients.
     * @param message Message to send.
     */
    NotificationListener(
        string type,
        vector<string> recipients,
        string message
    );

    /**
     * @brief Sends notifications in response to a workflow event.
     *
     * @param event Received workflow event.
     */
    void update(const WorkflowEvent& event) override;
};

/**
 * @brief Maintains a dashboard view of workflow status.
 *
 * Updates workflow progress information whenever
 * relevant workflow events are received.
 */
class DashboardListener : public WorkflowListener {
private:
    /**
     * @brief Stores workflow instance status information.
     */
    map<int, string> statusView;

public:
    /**
     * @brief Updates dashboard information based on an event.
     *
     * @param event Workflow event received from the publisher.
     */
    void update(const WorkflowEvent& event) override;

    /**
     * @brief Retrieves the status of a workflow instance.
     *
     * @param instanceId Unique workflow instance identifier.
     * @return Current status of the specified instance.
     */
    string getStatus(int instanceId) const;
};

#endif