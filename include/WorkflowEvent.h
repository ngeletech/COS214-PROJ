#ifndef WORKFLOWEVENT_H
#define WORKFLOWEVENT_H

#include <iostream>
#include <string>

/**
 * @brief Represents an event generated during workflow execution.
 *
 * WorkflowEvent carries information about an occurrence within a
 * workflow instance. It is used by the Observer pattern to notify
 * interested listeners such as AuditLogger, DashboardListener,
 * NotificationListener, and NextItemActivator.
 */
class WorkflowEvent {
private:

    /**
     * @brief Name of the event.
     */
    std::string eventName;

    /**
     * @brief Identifier of the workflow instance that generated the event.
     */
    int instanceId;

    /**
     * @brief Name of the workflow element that generated the event.
     */
    std::string elementName;

    /**
     * @brief Additional event information.
     */
    std::string details;

public:

    /**
     * @brief Constructs a WorkflowEvent.
     *
     * @param evName Name of the event.
     * @param id Workflow instance identifier.
     * @param elName Name of the workflow element.
     * @param details Additional event information.
     */
    WorkflowEvent(const std::string& evName, int id, 
        const std::string& elName, const std::string& details);

    /**
     * @brief Retrieves the event name.
     *
     * @return Event name.
     */
    std::string getEventName() const;

    /**
     * @brief Retrieves the workflow instance identifier.
     *
     * @return Instance identifier.
     */
    int getInstanceId() const;

    /**
     * @brief Retrieves the workflow element name.
     *
     * @return Element name.
     */
    std::string getElementName() const;

    /**
     * @brief Retrieves additional event details.
     *
     * @return Event details.
     */
    std::string getDetails() const;
};

#endif