#ifndef DECORATOR_H
#define DECORATOR_H

#include <string>
#include "WorkData.h"
#include "composite.h"

using namespace std;

class WorkElementDecorator: public WorkElement {
    protected:
        WorkElement* wrapped;
    
    public:
        WorkElementDecorator(WorkElement* element);
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

class PayloadDecorator: public WorkElementDecorator {
    private:
        string key;
        string value;

    public:
        PayloadDecorator(WorkElement* element, string key, string value);
        void execute();
        WorkData getPayload() const;
};

class AuditedDecorator: public WorkElementDecorator {
    public:
        AuditedDecorator(WorkElement* element);
        void complete();
        void execute();
};

class PriorityDecorator: public WorkElementDecorator {
    private:
        int priority;

    public:
        PriorityDecorator(WorkElement* element, int priority);
        void execute();
        int getPriority() const;
        void setPriority(int priority);
};

#endif //DECORATOR_H