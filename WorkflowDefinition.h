//Product
#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <string>
#include <vector>

// Forward declarations
class AssignmentStrategy;
class EscalationHandler;
struct WorkItemSpec;

/**
 * @class WorkflowDefinition
 * @brief Represents the immutable blueprint/product of a workflow definition.
 * 
 * Holds the static structure, stages, strategy rules, and escalation chain 
 * needed to create running workflow instances. Constructed exclusively via 
 * StandardDefinitionBuilder.
 */
class WorkflowDefinition {
private:
    std::string name;                          /**< The descriptive name of the workflow definition. */
    int version;                               /**< Version number of the definition for backward compatibility. */
    std::vector<WorkItemSpec> workItemSpecs;  /**< List of work item specifications included in this process. */
    AssignmentStrategy* assignmentStrategy;    /**< Strategy rule used to route and assign tasks. */
    EscalationHandler* escalationHandler;      /**< Head of the chain of responsibility for exception handling. */

    /**
     * @brief Private default constructor.
     * Enforces creation through StandardDefinitionBuilder.
     */
    WorkflowDefinition();

    /**
     * @brief Sets the version number of the workflow definition.
     * @param version The version number to assign.
     */
    void setVersion(int version);

    // Friend class declaration granting StandardDefinitionBuilder access to private members.
    friend class StandardDefinitionBuilder;

public:
    /**
     * @brief Destructor for WorkflowDefinition.
     */
    virtual ~WorkflowDefinition();

    /**
     * @brief Gets the name of the workflow definition.
     * @return std::string The workflow name.
     */
    std::string getName() const;

    /**
     * @brief Gets the version number of the workflow definition.
     * @return int The current version.
     */
    int getVersion() const;

    /**
     * @brief Gets the collection of work item specifications.
     * @return const std::vector<WorkItemSpec>& Reference to the vector of work item specs.
     */
    const std::vector<WorkItemSpec>& getWorkItemSpecs() const;

    /**
     * @brief Gets the assignment strategy attached to this definition.
     * @return AssignmentStrategy* Pointer to the assignment strategy object.
     */
    AssignmentStrategy* getAssignmentStrategy() const;

    /**
     * @brief Gets the head of the escalation handler chain.
     * @return EscalationHandler* Pointer to the first handler in the chain.
     */
    EscalationHandler* getEscalationHandler() const;
};

#endif // WORKFLOWDEFINITION_H