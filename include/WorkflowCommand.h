/**
 * @file WorkflowCommand.h
 * @brief Command pattern implementation for workflow operations.
 *
 * Defines the abstract WorkflowCommand interface and its concrete
 * implementations for performing operations on workflow instances.
 *
 * Pattern roles:
 *  - Command: WorkflowCommand
 *  - ConcreteCommand: StartInstanceCommand, CancelInstanceCommand,
 *                     ChangeDataCommand, ReassignItemCommand
 *  - Invoker: CommandInvoker
 *
 * CommandInvoker manages command execution and history, while the
 * RollbackManager uses the Memento pattern to restore previous workflow
 * instance states.
 */

#ifndef WORKFLOWCOMMAND_H
#define WORKFLOWCOMMAND_H

#include <iostream>
#include <queue>
#include <vector>
#include "WorkflowEngine.h"
#include "State.h"
#include "WorkData.h"

class RollbackManager;

/**
 * @class WorkflowCommand
 * @brief Abstract base class for workflow commands.
 *
 * Defines the common interface for commands that perform operations on
 * workflow instances. Stores a pointer to the target instance and requires
 * concrete commands to implement the execute() operation.
 *
 * The target pointer is non-owning. The command does not manage the
 * lifetime of the workflow instance.
 *
 * Pattern role:
 *  - Command: Defines the interface for executing operations.
 */
class WorkflowCommand {
    protected:
        WorkflowInstance* target; ///< Non-owning pointer to the target workflow instance.

    public:
        /**
         * @brief Constructs a command for a specific workflow instance.
         * @param target The workflow instance the command will operate on.
         */
        WorkflowCommand(WorkflowInstance* target);

        /** @brief Virtual destructor for safe deletion through the base class. */
        virtual ~WorkflowCommand();

        /**
         * @brief Executes the command.
         *
         * Must be implemented by concrete command classes.
         */
        virtual void execute() = 0;

        /**
         * @brief Gets the workflow instance targeted by the command.
         * @return Pointer to the target workflow instance.
         */
        WorkflowInstance* getTarget();
};

/**
 * @class StartInstanceCommand
 * @brief Command for starting a new workflow instance.
 *
 * Uses the WorkflowEngine to create a new instance from a workflow
 * definition and its initial data.
 *
 * This command has no target instance because it creates a new instance
 * rather than operating on an existing one.
 *
 * Pattern role:
 *  - ConcreteCommand: Implements the workflow-starting operation.
 */
class StartInstanceCommand : public WorkflowCommand {
    private:
        WorkflowEngine* engine; ///< Engine used to create the workflow instance.
        std::string defName;    ///< Name of the workflow definition to start.
        WorkData data;          ///< Initial data supplied to the new instance.

    public:
        /**
         * @brief Constructs a command to start a workflow instance.
         * @param engine Workflow engine used to create the instance.
         * @param defName Name of the workflow definition.
         * @param data Initial data for the new instance.
         */
        StartInstanceCommand(WorkflowEngine* engine, std::string defName, WorkData data);

        /**
         * @brief Creates and starts the requested workflow instance.
         */
        void execute() override;
};

/**
 * @class CancelInstanceCommand
 * @brief Command for cancelling an existing workflow instance.
 *
 * Encapsulates the cancellation operation by invoking cancel() on the
 * target workflow instance.
 *
 * Pattern role:
 *  - ConcreteCommand: Implements the workflow-cancellation operation.
 */
class CancelInstanceCommand : public WorkflowCommand {
    public:
        /**
         * @brief Constructs a command to cancel a workflow instance.
         * @param target The workflow instance to cancel.
         */
        CancelInstanceCommand(WorkflowInstance* target);

        /** @brief Cancels the target workflow instance. */
        void execute() override;
};

/**
 * @class ChangeDataCommand
 * @brief Command for modifying workflow instance data.
 *
 * Stores a data key and its replacement value. Execution updates the
 * corresponding entry in the target workflow instance.
 *
 * Pattern role:
 *  - ConcreteCommand: Implements the data-modification operation.
 */
class ChangeDataCommand : public WorkflowCommand {
    private:
        std::string key;   ///< Key of the data entry to modify.
        std::string value; ///< New value for the data entry.

    public:
        /**
         * @brief Constructs a command to change a data entry.
         * @param target The workflow instance whose data will be changed.
         * @param key The key of the data entry to modify.
         * @param value The new value to assign.
         */
        ChangeDataCommand(WorkflowInstance* target, std::string key, std::string value);

        /** @brief Updates the specified data entry in the target instance. */
        void execute() override;
};

/**
 * @class ReassignItemCommand
 * @brief Command for reassigning a workflow item to another participant.
 *
 * Finds the specified work item, updates its assignee and adjusts the
 * workload counters of the previous and new participants.
 *
 * The target instance and new assignee are stored as non-owning pointers.
 *
 * Pattern role:
 *  - ConcreteCommand: Implements the work-item reassignment operation.
 */
class ReassignItemCommand : public WorkflowCommand {
    private:
        std::string elementName; ///< Name of the work item to reassign.
        Participant* newAssignee; ///< Non-owning pointer to the new assignee.

    public:
        /**
         * @brief Constructs a command to reassign a work item.
         * @param target The workflow instance containing the item.
         * @param elementName Name of the work item to reassign.
         * @param newAssignee Participant who will become the new assignee.
         */
        ReassignItemCommand(WorkflowInstance* target, std::string elementName, Participant* newAssignee);

        /** @brief Reassigns the specified work item to the new participant. */
        void execute() override;
};

/**
 * @class CommandInvoker
 * @brief Manages command submission, execution and undo operations.
 *
 * Maintains a queue of pending commands and a history of successfully
 * executed commands. Before executing a command with a target instance,
 * it asks the RollbackManager to save a snapshot of that instance.
 *
 * During undo, the invoker retrieves the most recently executed command
 * and asks the RollbackManager to restore the corresponding instance.
 *
 * The invoker owns submitted command objects and deletes them when they
 * are destroyed, discarded after execution failure or removed during undo.
 *
 * Pattern role:
 *  - Invoker: Controls the execution of commands.
 */
class CommandInvoker {
    private:
        std::queue<WorkflowCommand*> queue; ///< Pending commands awaiting execution.
        std::vector<WorkflowCommand*> history; ///< Successfully executed commands.
        RollbackManager* rollback; ///< Rollback manager used to save and restore snapshots.

    public:
        /**
         * @brief Constructs the command invoker.
         * @param rollback Rollback manager used to save and restore snapshots.
         */
        CommandInvoker(RollbackManager* rollback);

        /** @brief Destroys the invoker and deletes its queued and historical commands. */
        ~CommandInvoker();

        /**
         * @brief Adds a command to the execution queue.
         * @param command Command to submit. Ownership passes to the invoker.
         */
        void submit(WorkflowCommand* command);

        /** @brief Executes the next command in the queue, if one is available. */
        void runNext();

        /** @brief Undoes the most recently executed command when a snapshot is available. */
        void undoLast();
};

#endif /* WORKFLOWCOMMAND_H */