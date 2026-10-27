#include "Chain-Of-Responsibility.h"

#include "Issue.h"
#include <iostream>

SupervisorHandler::SupervisorHandler(Participant* responsible, int maxSeverity)
    : EscalationHandler(responsible, maxSeverity)
{
}

void SupervisorHandler::resolve(const Issue& issue)
{
    std::cout << "Supervisor Handler resolved issue: "
         << issue.getDescription() << std::endl;
}