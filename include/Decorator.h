#ifndef DECORATOR_H
#define DECORATOR_H

#include <string>
using namespace std;

class WorkElementDecorator: public WorkElement {
    private:
        WorkElement* wrapped;
    
    public:
        WorkElementDecorator(WorkElement* element);
        ~WorkElementDecorator();
        void execute();
        void complete();
        bool isComplete();
        bool isAvailable();
        WorkData getPayload();
        string getName();
        WorkItemStatus getstatus();
        void setStatus(WorkItemStatus status);
        WorkflowInstance* getInstance();
        void setInstance(WorkflowInstance* instance);
        void assignTo(Participant* participant);
};

class PayloadDecorator: WorkElementDecorator {
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

class PriorityDecorator: WorkElementDecorator {
    private:
        int priority;

    public:
        PriorityDecorator(WorkElement* element, int priority);
        int getPriority();
        void setPriority(int priority);
};

#endif //DECORATOR_H