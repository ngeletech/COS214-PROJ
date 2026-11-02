/**
 * @file Builder.h
 * @brief Unified header for the Builder design pattern components.
 */

#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <vector>
#include <stdexcept>

#include "AssignmentStrategy.h"
#include "Chain-Of-Responsibility.h"

// Forward declarations
class WorkflowDefinitionBuilder;
class WorkflowDefinition;
class AssignmentStrategy;
class EscalationHandler;

struct WorkItemSpec {
    std::string type;
    std::string name;
    std::string config;
};

//Builder

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

    /**
     * @brief Resets the builder to its initial state, discarding any partially built product.
     */
    virtual void reset() = 0;
};


//Product

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
    friend class WorkflowEngine;

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



//Concrete Builder


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

    /**
     * @brief Resets the builder to its initial state, discarding any partially built product.
     */
    void reset() override;

};

//Director

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
                    
    ~WorkflowDesigner();
};

#endif // BUILDER_H