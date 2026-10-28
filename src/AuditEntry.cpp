#include "../include/Iterator.h"

#include <string>

AuditEntry::AuditEntry(int instanceId, const std::string& eventType, const std::string& details){
    this->instanceId = instanceId;
    this->eventType = eventType;
    this->details = details;
}

int AuditEntry::getInstanceId() const {
    return instanceId;
}

std::string AuditEntry::getEventType() const {
    return eventType;
}

std::string AuditEntry::getDetails() const {
    return details;
}
