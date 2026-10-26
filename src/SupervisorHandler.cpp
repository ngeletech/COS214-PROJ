#include "Chain-Of-Responsibility.h"

#include "Issue.h"
#include <iostream>

using namespace std;

SupervisorHandler::SupervisorHandler(Participant* responsible, int maxSeverity)
    : EscalationHandler(responsible, maxSeverity)
{
}

void SupervisorHandler::resolve(const Issue& issue)
{
    cout << "Supervisor Handler resolved issue: "
         << issue.getDescription() << endl;
}