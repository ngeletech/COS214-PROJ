#include "AssignmentStrategy.h"

Participant* AssignmentService::assignWorkItem(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) {
    return strategy->select(element, data, candidates);
}