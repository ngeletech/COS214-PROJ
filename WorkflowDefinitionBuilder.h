//Builder
#ifndef WORKFLOWDEFINITIONBUILDER_H
#define WORKFLOWDEFINITIONBUILDER_H

#include <string>

// Forward declarations
class WorkflowDefinition;
class AssignmentStrategy;
class EscalationHandler;
struct WorkItemSpec;

/**
 * @class WorkflowDefinitionBuilder
 * @brief Abstract Builder interface for constructing WorkflowDefinition objects.
 * 
 * Declares the abstract steps required to build a complex WorkflowDefinition instance.
 */
class WorkflowDefinitionBuilder {
public:
    /**
     * @brief Virtual destructor for WorkflowDefinitionBuilder.
     */
    virtual ~WorkflowDefinitionBuilder() {}

    /**
     * @brief Sets the name of the workflow definition under construction.
     * @param name The name to assign to the workflow.
     */
    virtual void setName(const std::string& name) = 0;

    /**
     * @brief Adds a work item specification to the workflow definition.
     * @param spec The WorkItemSpec structure defining the task.
     */
    virtual void addWorkItem(const WorkItemSpec& spec) = 0;

    /**
     * @brief Configures the assignment strategy rule for task routing.
     * @param strategy Pointer to the concrete AssignmentStrategy instance.
     */
    virtual void setAssignmentRule(AssignmentStrategy* strategy) = 0;

    /**
     * @brief Configures the escalation handler chain for exception resolving.
     * @param handler Pointer to the head of the EscalationHandler chain.
     */
    virtual void setEscalationChain(EscalationHandler* handler) = 0;

    /**
     * @brief Assembles and returns the completed WorkflowDefinition object.
     * @return WorkflowDefinition* Pointer to the constructed workflow definition product.
     */
    virtual WorkflowDefinition* build() = 0;
};

#endif // WORKFLOWDEFINITIONBUILDER_H