#include "AssignmentStrategy.h"

RoleMatchingStrategy::RoleMatchingStrategy(std::string role) : requiredRole(role) {}
Participant* RoleMatchingStrategy::select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) {
    std::string role = requiredRole;
    std::map<std::string, std::string>::const_iterator it = roleByItem.find(element.getName());
    if(it != roleByItem.end()) {
        role = it->second;
    }
    for (std::size_t i = 0; i < candidates.size(); i++) {
        Participant* participant = candidates.at(i);
        if (participant->getRole() == role) {
            return participant;
        }
    }
    return nullptr;
}

void RoleMatchingStrategy::requireRoleFor(const std::string& itemName, const std::string& role) {
    roleByItem[itemName] = role;
}