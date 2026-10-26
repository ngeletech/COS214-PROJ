#ifndef DECORATOR_H
#define DECORATOR_H

#include "composite.h"
#include <string>
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
        bool isAvailable();
        WorkData getPayload() const;
        string getName();
        WorkItemStatus getStatus();
        void setStatus(WorkItemStatus status);
        WorkflowInstance* getInstance();
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
        WorkData getPayload();
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
        int getPriority();
        void setPriority(int priority);
};

#endif //DECORATOR_H