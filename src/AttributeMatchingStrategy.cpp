#include "AssignmentStrategy.h"

AttributeMatchingStrategy::AttributeMatchingStrategy(std::string name, std::string value) : dataKey(name), participantAttribute(value) {}
Participant* AttributeMatchingStrategy::select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) {
    for (std::size_t i = 0; i < candidates.size(); i++) {
        Participant* participant = candidates.at(i);
        if (participant->getAttribute(dataKey) == participantAttribute) {
            return participant;
        }
    }
    return nullptr;
}