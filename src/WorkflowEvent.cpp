#include "WorkflowEvent.h"

WorkflowEvent::WorkflowEvent(const std::string& evName, int id,
    const std::string& elName, const std::string& details)
    : eventName(evName),
      instanceId(id),
      elementName(elName),
      details(details)
{
}

std::string WorkflowEvent::getEventName() const
{
    return eventName;
}

int WorkflowEvent::getInstanceId() const
{
    return instanceId;
}

std::string WorkflowEvent::getElementName() const
{
    return elementName;
}

std::string WorkflowEvent::getDetails() const
{
    return details;
}