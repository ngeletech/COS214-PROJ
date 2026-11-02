#include "Observer.h"
#include "WorkflowEvent.h"

// ===============================
// WorkflowEventPublisher
// ===============================

void WorkflowEventPublisher::subscribe(WorkflowListener* listener)
{
    listeners.push_back(listener);
}

void WorkflowEventPublisher::publish(const WorkflowEvent& event)
{
    for (WorkflowListener* listener : listeners){
        listener->update(event);
    }
}

WorkflowEventPublisher::~WorkflowEventPublisher()
{
}