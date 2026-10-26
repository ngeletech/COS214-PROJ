#include "Chain-Of-Responsibility.h"

#include "Issue.h"
#include <iostream>

using namespace std;

FrontlineHandler::FrontlineHandler(Participant* responsible, int maxSeverity)
    : EscalationHandler(responsible, maxSeverity)
{
}

void FrontlineHandler::resolve(const Issue& issue)
{
    cout << "Frontline Handler resolved issue: "
         << issue.getDescription() << endl;
}