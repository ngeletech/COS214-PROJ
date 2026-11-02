#include "Observer.h"
#include "WorkflowEngine.h"
#include "WorkflowEvent.h"

// ===============================
// NextItemActivator
// ===============================

NextItemActivator::NextItemActivator(WorkflowEngine* engine) : engine(engine)
{
}

void NextItemActivator::update(const WorkflowEvent& event)
{
    if (event.getEventName() == "TASK_COMPLETED"){
        // Retrieve the workflow instance that
        // generated the event.
        WorkflowInstance* instance = engine->getInstance(event.getInstanceId());

        if (instance != nullptr){
            engine->activateNext(*instance);
        }
    }
}