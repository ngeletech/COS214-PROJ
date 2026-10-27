#include "Chain-Of-Responsibility.h"

#include "Issue.h"
#include <iostream>

ManagerHandler::ManagerHandler(Participant* responsible, int maxSeverity)
    : EscalationHandler(responsible, maxSeverity)
{
}

void ManagerHandler::resolve(const Issue& issue)
{
    std::cout << "Manager Handler resolved issue: "
         << issue.getDescription() << std::endl;
}