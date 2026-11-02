#ifndef DECORATOR_H
#define DECORATOR_H

#include <string>
#include "WorkData.h"
#include "composite.h"

using namespace std;

/**
 * @class WorkElementDecorator
 * @brief Base Decorator: forwards all requests to the wrapped WorkElement
 * 
 * Holds a pointer to the wrapped component and provides the default forwarding implementations for the entire the WorkElement interface.
 */

class WorkElementDecorator: public WorkElement {
    protected:
        /**
         * @brief The element being the decorated
         */
        WorkElement* wrapped;
    
    public:
        /**
         * @brief Wraps the passed through element
         * @param element The element to decorate. Takes ownership
         */
        WorkElementDecorator(WorkElement* element);

        /**
         * @brief Deleted the wrapped element
         */
        virtual ~WorkElementDecorator();

        /**
         * @brief Forwards the execution to the wrapped element
         */
        void execute();

        /**
         * @brief Forwards completion to the wrapped element
         */
        void complete();

        /**
         * @brief Checks if the wrapped element is complete
         * @return Ture if complete, false otherwise
         */
        bool isComplete() const;

        /**
         * @brief Checks if the wrapped element is available
         * @return Ture if available, false otherwise
         */
        bool isAvailable() const;

        /**
         * @brief  Returns the payload of the wrapped element
         * @return A copy of the WorkData payload
         */
        WorkData getPayload() const;

        /** 
         * @brief Returns the name of the wrapped element.
         * @return The name string.
         */
        string getName() const;

        /**
         * @brief Returns the status of the wrapped element.
         * @return The current WorkItemStatus.
         */
        WorkItemStatus getStatus() const;

        /**
         * @brief Sets the status of the wrapped element.
         * @param status The new status.
         */
        void setStatus(WorkItemStatus status);

        /**
         * @brief Returns the owning instance of the wrapped element.
         * @return Pointer to the WorkflowInstance, or nullptr if not set.
         */
        WorkflowInstance* getInstance() const;

        /**
         * @brief Sets the owning instance of the wrapped element.
         * @param instance The WorkflowInstance to set.
         */
        void setInstance(WorkflowInstance* instance);

        /**
         * @brief Assigns a participant to the wrapped element.
         * @param participant The participant to assign.
         */
        void assignTo(Participant* participant);

        /**
         * @brief Sets the parent group of the wrapped element.
         * @param parent The WorkGroup to set as parent.
         */
        void setParent(WorkGroup* parent);

        /**
         * @brief Returns the wrapped element as a group or nullptr
         */
        WorkGroup* asGroup();

        /**
         * @brief Returns the assigned participant of the wrapped element.
         * @return Pointer to the Participant, or nullptr if none.
         */
        Participant* getAssignee() const;

        /**
         * @brief Checks if the wrapped element requires an assignee.
         * @return True if an assignee is required, false otherwise.
         */
        bool requiresAssignee() const;

        /**
         * @brief Returns the priority of the wrapped element 
         * @return The priority, or 0 if nothig is wrapped.
         */
        int getPriority() const;
};

/**
 * @class PayloadDecorator
 * @brief Adds a key-value pair to the payload returned by the wrapped element
 */
class PayloadDecorator: public WorkElementDecorator {
    private:
        /**
         * @brief The key to add to the payload
         */
        string key;

        /**
         * @brief The value to add to the payload
         */
        string value;

    public:
        /**
         * @brief Wraps the element and stores the key-value pair
         * @param element The element being decorated
         * @param key The payload key
         * @param value The payload value
         */
        PayloadDecorator(WorkElement* element, string key, string value);

        /**
         * @brief Runs the wrapped element
         */
        void execute();

        /**
         * @brief Returns a copy of the wrapped payload with the new entry added
         * @return A WorkData payload reference
         */
        WorkData getPayload() const;
};

/**
 * @class AuditedDecorator
 * @brief Prints audit messages before and adter executing or completing the wrapped element
 */

class AuditedDecorator: public WorkElementDecorator {
    public:
        /**
         * @brief Wraps the given element
         * @param element The element to decorate
         */
        AuditedDecorator(WorkElement* element);

        /**
         * @brief Prints "before completion", runs the wrapped element, then prints "after completion"
         */
        void complete();

        /**
         * @brief Prints "before execution", runs the wrapped element, then prints "after execution"
         */
        void execute();
};

/**
 * @class PriorityDecorator
 * @brief Assigns the wrapped element a priority
 */
class PriorityDecorator: public WorkElementDecorator {
    private:
        /**
         * @brief The priority of the wrapped element
         */
        int priority;

    public:
        /**
         * @brief Wraps the element and assigns a priority
         * @param element The element to decorate
         * @param priority The priority value
         */
        PriorityDecorator(WorkElement* element, int priority);

        /**
         * @brief Runs the wrapped element with a new priority
         */
        void execute();

        /**
         * @brief Returns the priority of the wrapped element
         * @return A priority 
         */
        int getPriority() const;

        /**
         * @brief Updates the priority
         * @param priority The new priority value
         */
        void setPriority(int priority);
};

#endif //DECORATOR_H