#include "Chain-Of-Responsibility.h"
#include "Issue.h"
#include "Participant.h"

EscalationHandler::EscalationHandler(Participant* responsible, int maxSeverity)
    : next(nullptr),
      responsible(responsible),
      maxSeverity(maxSeverity)
{
}

EscalationHandler::~EscalationHandler()
{
}

void EscalationHandler::setNext(EscalationHandler* handler)
{
    next = handler;
}

bool EscalationHandler::handleIssue(const Issue& issue)
{
    if (issue.getSeverity() <= maxSeverity){
        resolve(issue);
        return true;
    }

    if (next != nullptr){
        return next->handleIssue(issue);
    }

    return false;
}