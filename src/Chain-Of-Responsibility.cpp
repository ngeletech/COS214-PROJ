#include "Chain-Of-Responsibility.h"

// ===============================
// EscalationHandler
// ===============================

EscalationHandler::EscalationHandler(
    Participant* responsible,
    int maxSeverity)
{
}

EscalationHandler::~EscalationHandler()
{
}

void EscalationHandler::setNext(
    EscalationHandler* handler)
{
}

bool EscalationHandler::handleIssue(
    const Issue& issue)
{
    return false;
}

// ===============================
// FrontlineHandler
// ===============================

FrontlineHandler::FrontlineHandler(
    Participant* responsible,
    int maxSeverity)
    : EscalationHandler(
        responsible,
        maxSeverity)
{
}

void FrontlineHandler::resolve(
    const Issue& issue)
{
}

// ===============================
// SupervisorHandler
// ===============================

SupervisorHandler::SupervisorHandler(
    Participant* responsible,
    int maxSeverity)
    : EscalationHandler(
        responsible,
        maxSeverity)
{
}

void SupervisorHandler::resolve(
    const Issue& issue)
{
}

// ===============================
// ManagerHandler
// ===============================

ManagerHandler::ManagerHandler(
    Participant* responsible,
    int maxSeverity)
    : EscalationHandler(
        responsible,
        maxSeverity)
{
}

void ManagerHandler::resolve(
    const Issue& issue)
{
}