#include "Observer.h"
#include "WorkflowEvent.h"

// ===============================
// DashboardListener
// ===============================

void DashboardListener::update(const WorkflowEvent& event)
{
    statusView[event.getInstanceId()] = event.getEventName();
}

std::string DashboardListener::getStatus(int instanceId) const
{
    auto it = statusView.find(instanceId);

    if (it != statusView.end()){
        return it->second;
    }

    return "Unknown";
}