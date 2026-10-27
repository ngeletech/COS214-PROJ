#include "Observer.h"
#include "Iterator.h"
#include "WorkflowEvent.h"

// ===============================
// AuditLogger
// ===============================

AuditLogger::AuditLogger(AuditHistory* history) : history(history)
{
}

void AuditLogger::update(const WorkflowEvent& event)
{
    AuditEntry entry( event.getInstanceId(), 
                      event.getEventName(), 
                      event.getDetails());

    history->append(entry);
}