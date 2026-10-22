#include "Observer.h"

// ===============================
// WorkflowEventPublisher
// ===============================

void WorkflowEventPublisher::subscribe(WorkflowListener* listener)
{
}

void WorkflowEventPublisher::publish(const string& event)
{
}

WorkflowEventPublisher::~WorkflowEventPublisher()
{
}

// ===============================
// AuditLogger
// ===============================

AuditLogger::AuditLogger(AuditHistory* history)
{
}

void AuditLogger::update(const WorkflowEvent& event)
{
}

// ===============================
// NextItemActivator
// ===============================

NextItemActivator::NextItemActivator(WorkflowEngine* engine)
{
}

void NextItemActivator::update(const WorkflowEvent& event)
{
}

// ===============================
// NotificationListener
// ===============================

NotificationListener::NotificationListener(
    string type,
    vector<string> recipients,
    string message)
{
}

void NotificationListener::update(const WorkflowEvent& event)
{
}

// ===============================
// DashboardListener
// ===============================

void DashboardListener::update(const WorkflowEvent& event)
{
}

string DashboardListener::getStatus(int instanceId) const
{
    return "";
}