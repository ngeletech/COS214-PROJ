#include "Participant.h"

Participant::Participant(int id, const std::string& name, const std::string& role, ExternalServiceConnector* connector)
    : id(id), name(name), role(role), workload(0), connector(connector) {}

int Participant::getId() const { 
    return id; 
}

const std::string& Participant::getName() const { 
    return name; 
}

const std::string& Participant::getRole() const { 
    return role; 
}

int Participant::getWorkload() const { 
    return workload; 
}

void Participant::incrementWorkload() { 
    ++workload; 
}

void Participant::decrementWorkload() { 
    if (workload > 0) --workload; 
}

ExternalServiceConnector* Participant::getConnector() const {
    return connector; 
}

void Participant::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string Participant::getAttribute(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = attributes.find(key);
    return it == attributes.end() ? "" : it->second;
}

bool Participant::hasAttribute(const std::string& key) const {
    return attributes.count(key) > 0;
}