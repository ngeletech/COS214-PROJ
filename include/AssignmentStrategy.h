#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <vector>
#include <map>
#include "WorkData.h"
#include "Participant.h"
#include "composite.h"



class AssignmentStrategy {
    public:
        virtual Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) = 0;
        virtual ~AssignmentStrategy() {}
};

class RoleMatchingStrategy : public AssignmentStrategy {
    private:
        std::string requiredRole;
    public:
        RoleMatchingStrategy(std::string role);
        Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) override;
};

class AttributeMatchingStrategy : public AssignmentStrategy {
    private:
        std::string dataKey;
        std::string participantAttribute;
    public:
        AttributeMatchingStrategy(std::string name, std::string value);
        Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) override;
};

class LeastLoadedStrategy : public AssignmentStrategy {
    public:
        Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) override;
};

class AssignmentService {
    private:
        AssignmentStrategy* strategy;
    public:
        AssignmentService(AssignmentStrategy* strategy) : strategy(strategy) {}
        Participant* assignWorkItem(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates);
};

#endif /* ASSIGNMENTSTRATEGY_H */