#include "Chain-Of-Responsibility.h"

#include "Issue.h"
#include <iostream>

using namespace std;

ManagerHandler::ManagerHandler(Participant* responsible, int maxSeverity)
    : EscalationHandler(responsible, maxSeverity)
{
}

void ManagerHandler::resolve(const Issue& issue)
{
    cout << "Manager Handler resolved issue: "
         << issue.getDescription() << endl;
}