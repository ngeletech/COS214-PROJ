/**
 * @file AssignmentStrategy.h
 * @brief Declares the Strategy pattern used to select participants for work assignment.
 *
 * Pattern roles:
 * - Strategy: AssignmentStrategy
 * - Concrete Strategies: RoleMatchingStrategy, AttributeMatchingStrategy,
 *   LeastLoadedStrategy
 * - Context: AssignmentService
 */

#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <vector>
#include <map>
#include "WorkData.h"
#include "composite.h"
#include "Participant.h"

/**
 * @class AssignmentStrategy
 * @brief Abstract strategy interface for selecting a participant to perform a work item.
 *
 * Concrete strategies implement different participant-selection algorithms.
 * The selected participant is returned to the caller; this interface does
 * not perform the actual work assignment.
 */
class AssignmentStrategy {
    public:
        /**
         * @brief Selects a participant from the available candidates.
         *
         * @param element Work element for which a participant is being selected.
         * @param data Data associated with the work item.
         * @param candidates List of participants considered for assignment.
         * @return Pointer to the selected participant, or nullptr if no
         *         suitable candidate is found.
         */
        virtual Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) = 0;

        /** @brief Virtual destructor for safe destruction through the base class. */
        virtual ~AssignmentStrategy() {}
};

/**
 * @class RoleMatchingStrategy
 * @brief Selects a participant whose role matches a required role.
 *
 * Iterates through the candidate list and returns the first participant
 * whose role matches the role supplied to the constructor.
 */
class RoleMatchingStrategy : public AssignmentStrategy {
    private:
        std::string requiredRole; ///< Role that a candidate must have to be selected.
        std::map<std::string, std::string> roleByItem; 

    public:
        /**
         * @brief Constructs a role-based assignment strategy.
         * @param role Role required for a participant to be selected.
         */
        RoleMatchingStrategy(std::string role);

        /**
         * @brief Selects the first candidate with the required role.
         *
         * @param element Work element for which a participant is being selected.
         * @param data Data associated with the work item.
         * @param candidates Participants to search through.
         * @return First participant whose role matches requiredRole,
         *         or nullptr if no candidate matches.
         */
        Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) override;

        void requireRoleFor(const std::string& itemName, const std::string& role);
};

/**
 * @class AttributeMatchingStrategy
 * @brief Selects a participant by matching one of their attributes.
 *
 * Searches the candidate list and returns the first participant whose
 * specified attribute has the required value.
 */
class AttributeMatchingStrategy : public AssignmentStrategy {
    private:
        std::string dataKey;              ///< Attribute key to retrieve from each candidate.
        std::string participantAttribute; ///< Attribute value required for selection.

    public:
        /**
         * @brief Constructs an attribute-based assignment strategy.
         *
         * @param name Attribute key used to retrieve a candidate's attribute.
         * @param value Attribute value required for a match.
         */
        AttributeMatchingStrategy(std::string name, std::string value);

        /**
         * @brief Selects the first candidate with a matching attribute.
         *
         * @param element Work element for which a participant is being selected.
         * @param data Data associated with the work item.
         * @param candidates Participants to search through.
         * @return First participant whose attribute matches the required value,
         *         or nullptr if no candidate matches.
         *
         * @note A missing participant attribute is treated as an empty string
         *       by Participant::getAttribute().
         */
        Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) override;
};

/**
 * @class LeastLoadedStrategy
 * @brief Selects the candidate with the lowest current workload.
 *
 * Iterates through the candidates and keeps the participant with the
 * smallest workload. If multiple candidates have the same lowest workload,
 * the first one encountered is selected.
 */
class LeastLoadedStrategy : public AssignmentStrategy {
    public:
        /**
         * @brief Selects the least-loaded participant.
         *
         * @param element Work element for which a participant is being selected.
         * @param data Data associated with the work item.
         * @param candidates Participants whose workloads are compared.
         * @return Participant with the lowest workload, or nullptr if the
         *         candidate list is empty.
         */
        Participant* select(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates) override;
};

/**
 * @class AssignmentService
 * @brief Context class that uses an assignment strategy to select participants.
 *
 * Maintains a pointer to the currently selected AssignmentStrategy.
 * The strategy can be changed at runtime, allowing different participant-
 * selection algorithms to be used without changing the service itself.
 */
class AssignmentService {
    private:
        AssignmentStrategy* strategy; ///< Strategy currently used for participant selection.

    public:
        /**
         * @brief Constructs the assignment service with an initial strategy.
         * @param strategy Strategy used to select participants.
         */
        AssignmentService(AssignmentStrategy* strategy) : strategy(strategy) {}

        /**
         * @brief Replaces the currently configured assignment strategy.
         * @param newStrategy Strategy to use for subsequent selections.
         */
        void setStrategy(AssignmentStrategy* newStrategy) { strategy = newStrategy; }

        /**
         * @brief Returns the currently configured assignment strategy.
         * @return Pointer to the current strategy.
         */
        AssignmentStrategy* getStrategy() const { return strategy; }

        /**
         * @brief Selects a participant by delegating to the current strategy.
         *
         * @param element Work element for which a participant is being selected.
         * @param data Data associated with the work item.
         * @param candidates Participants available for selection.
         * @return Participant selected by the configured strategy.
         *
         * @note This method does not assign the work item to the returned
         *       participant; it only delegates participant selection.
         */
        Participant* assignWorkItem(WorkElement& element, const WorkData& data, std::vector<Participant*> candidates);
};

#endif /* ASSIGNMENTSTRATEGY_H */