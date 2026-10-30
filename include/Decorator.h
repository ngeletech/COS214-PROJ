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
        ~WorkElementDecorator();
        void execute();
        void complete();
        bool isComplete() const;
        bool isAvailable() const;
        WorkData getPayload() const;
        string getName() const;
        WorkItemStatus getStatus() const;
        void setStatus(WorkItemStatus status);
        WorkflowInstance* getInstance() const;
        void setInstance(WorkflowInstance* instance);
        void assignTo(Participant* participant);
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