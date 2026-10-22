//Director
#ifndef WORKFLOWDESIGNER_H
#define WORKFLOWDESIGNER_H

#include <string>
#include <vector>

// Forward declarations
class WorkflowDefinitionBuilder;
class WorkflowDefinition;
class AssignmentStrategy;
class EscalationHandler;
struct WorkItemSpec;

/**
 * @class WorkflowDesigner
 * @brief Director class in the Builder pattern.
 * 
 * Controls the assembly process by driving a WorkflowDefinitionBuilder instance through 
 * a specific sequence of construction steps to create a ready-to-use WorkflowDefinition.
 */
class WorkflowDesigner {
private:
    WorkflowDefinitionBuilder* builder; /**< Pointer to the builder interface used for construction. */

public:
    /**
     * @brief Construct a new WorkflowDesigner object with a specified builder.
     * @param builder Pointer to the concrete or abstract WorkflowDefinitionBuilder.
     */
    WorkflowDesigner(WorkflowDefinitionBuilder* builder);

    /**
     * @brief Executes the step-by-step construction sequence to build a WorkflowDefinition.
     * 
     * @param name The overall name of the workflow definition.
     * @param specs Vector of task/item specifications to include.
     * @param strategy The assignment strategy rule to attach.
     * @param chain The escalation chain of responsibility to attach.
     * @return WorkflowDefinition* Pointer to the finalized workflow definition instance.
     */
    WorkflowDefinition* construct(const std::string& name,
                                 const std::vector<WorkItemSpec>& specs,
                                 AssignmentStrategy* strategy,
                                 EscalationHandler* chain);
};

#endif // WORKFLOWDESIGNER_H