//Concrete Builder
#ifndef STANDARDDEFINITIONBUILDER_H
#define STANDARDDEFINITIONBUILDER_H

#include "WorkflowDefinitionBuilder.h"

/**
 * @class StandardDefinitionBuilder
 * @brief Concrete Builder implementation for constructing generic WorkflowDefinition instances.
 * 
 * Implements the builder interface steps to populate and configure a WorkflowDefinition product.
 */
class StandardDefinitionBuilder : public WorkflowDefinitionBuilder {
private:
    WorkflowDefinition* product; /**< Pointer to the WorkflowDefinition instance currently being built. */

public:
    /**
     * @brief Constructor that initializes a fresh WorkflowDefinition product instance.
     */
    StandardDefinitionBuilder();

    /**
     * @brief Destructor for StandardDefinitionBuilder.
     */
    virtual ~StandardDefinitionBuilder();

    /**
     * @brief Sets the name of the workflow definition product.
     * @param name The name string.
     */
    void setName(const std::string& name) override;

    /**
     * @brief Appends a work item specification to the product.
     * @param spec The work item specification to add.
     */
    void addWorkItem(const WorkItemSpec& spec) override;

    /**
     * @brief Sets the task assignment strategy on the product.
     * @param strategy Pointer to the assignment strategy to assign.
     */
    void setAssignmentRule(AssignmentStrategy* strategy) override;

    /**
     * @brief Sets the escalation handler chain on the product.
     * @param handler Pointer to the head of the escalation chain.
     */
    void setEscalationChain(EscalationHandler* handler) override;

    /**
     * @brief Finalizes and returns the completed WorkflowDefinition product.
     * @return WorkflowDefinition* The fully built, validated workflow definition.
     */
    WorkflowDefinition* build() override;
};

#endif // STANDARDDEFINITIONBUILDER_H