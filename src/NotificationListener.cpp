#include "Observer.h"
#include "WorkflowEvent.h"
#include <iostream>

// ===============================
// NotificationListener
// ===============================

NotificationListener::NotificationListener(
    std::string type,
    std::vector<std::string> recipients,
    std::string message) : eventType(type),
                           recipients(recipients),
                           message(message)
{
}

void NotificationListener::update(const WorkflowEvent& event)
{
    if (event.getEventName() == eventType){
        for (const std::string& recipient : recipients){
            std::cout << "Notification sent to " 
                 << recipient << ": " 
                 << message << std::endl;
        }
    }
}