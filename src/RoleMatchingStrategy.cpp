#include "AssignmentStrategy.h"

RoleMatchingStrategy::RoleMatchingStrategy(std::string role) : requiredRole(role) {}
Participant* RoleMatchingStrategy::select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) {
    for (std::size_t i = 0; i < candidates.size(); i++) {
        Participant* participant = candidates.at(i);
        if (participant->getRole() == requiredRole) {
            return participant;
        }
    }
    return nullptr;
}
