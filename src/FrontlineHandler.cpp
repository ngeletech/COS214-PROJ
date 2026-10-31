#include "Chain-Of-Responsibility.h"

#include "Issue.h"
#include <iostream>

FrontlineHandler::FrontlineHandler(Participant* responsible, int maxSeverity)
    : EscalationHandler(responsible, maxSeverity)
{
}

void FrontlineHandler::resolve(const Issue& issue)
{
    std::cout << "Frontline Handler resolved issue: "
         << issue.getDescription() << std::endl;
}