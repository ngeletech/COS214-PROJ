#include "AssignmentStrategy.h"

Participant* LeastLoadedStrategy::select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) {
    Participant* leastLoaded = nullptr;
    for (std::size_t i = 0; i < candidates.size(); i++) {
        Participant* participant = candidates.at(i);
        if (leastLoaded == nullptr || participant->getWorkload() < leastLoaded->getWorkload()) {
            leastLoaded = participant;
        }
    }
    return leastLoaded;
}   