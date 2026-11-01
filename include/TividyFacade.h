#ifndef TIVIDYFACADE_H
#define TIVIDYFACADE_H

#include <string>
#include <vector>
#include "WorkData.h"
#include "Builder.h"

class WorkflowDefinition;
class AssignmentStrategy;
class EscalationHandler;
class Participant;
class WorkflowListener;
class WorkflowCommand;
class WorkflowInstance;
class AuditHistory;
class RollbackManager;
class CommandInvoker;
class AssignmentService;
class WorkItemFactoryRegistry;
class WorkflowEngine;
class WorkflowDesigner;
class AuditLogger;
class NextItemActivator;

/**
 * @class TividyFacade
 * @brief Facade. The single entry point for clients (the TUI).
 *
 * Creates and wires the engine and its helpers, and hides them behind a few
 * simple operations. Owns everything it creates, plus the participants and
 * listeners passed to it.
 *
 * Invalid requests throw: std::invalid_argument for unknown ids or names,
 * std::logic_error for actions the instance's state refuses. The caller
 * catches them.
 */
class TividyFacade {
private:
    AuditHistory* audit;                        ///< Audit trail (owned).
    RollbackManager* rollback;                  ///< Stores snapshots for undo (owned).
    CommandInvoker* invoker;                    ///< Runs commands and keeps their history (owned).
    AssignmentService* assignment;              ///< Picks participants for work items (owned).
    AssignmentStrategy* defaultStrategy;        ///< Strategy used by the assignment service (owned).
    WorkItemFactoryRegistry* factories;         ///< Factories for each work item type (owned).
    WorkflowEngine* engine;                     ///< Runs workflow instances (owned).
    WorkflowDesigner* designer;                 ///< Builds workflow definitions (owned).
    AuditLogger* auditLogger;                   ///< Default listener that records events (owned).
    NextItemActivator* activator;               ///< Default listener that activates next items (owned).
    std::vector<Participant*> participants;     ///< Registered participants (owned).
    std::vector<WorkflowListener*> listeners;   ///< Listeners added by the client (owned).
    std::vector<std::string> definitionNames;   ///< Names of defined workflows, in definition order.

    TividyFacade(const TividyFacade&);                 ///< Not copyable.
    TividyFacade& operator=(const TividyFacade&);      ///< Not copyable.

    /**
     * @brief Looks up an instance or throws.
     * @param instanceId The instance id.
     * @return The instance.
     * @throws std::invalid_argument if the id is unknown.
     */
    WorkflowInstance& requireInstance(int instanceId) const;

    /**
     * @brief Submits a command to the invoker and runs it.
     * @param command The command. The invoker takes ownership.
     */
    void run(WorkflowCommand* command);

public:
    /** @brief Creates and wires the engine and all its helpers. */
    TividyFacade();

    /** @brief Deletes everything the facade owns, engine first. */
    ~TividyFacade();

    /**
     * @brief Builds a workflow definition and registers it with the engine.
     * @param name Name of the workflow.
     * @param specs The work items, in order.
     * @param strategy Must be heap-allocated and not shared with another definition.
     *        Ownership passes to the definition, which deletes it. The engine
     *        currently assigns using the facade's default strategy.
     * @param chain Head of the escalation chain. Must be heap-allocated and not shared.
     *        Ownership of the head passes to the definition, which deletes it.
     *        Handlers linked after the head are not deleted.
     * @return The new definition (owned by the engine).
     */
    WorkflowDefinition* defineWorkflow(const std::string& name, const std::vector<WorkItemSpec>& specs, AssignmentStrategy* strategy, EscalationHandler* chain);

    /**
     * @brief Lists the names of the defined workflows.
     * @return The names, in the order they were defined.
     */
    std::vector<std::string> listDefinitions() const;

    /**
     * @brief Starts a new instance of a workflow.
     * @param defName Name of a defined workflow.
     * @param data Initial data for the instance.
     * @return The id of the new instance.
     * @throws std::invalid_argument if the workflow is unknown or cannot be built.
     */
    int startWorkflow(const std::string& defName, const WorkData& data);

    /**
     * @brief Cancels an instance. Can be undone with undoLast().
     * @param instanceId The instance id.
     * @throws std::invalid_argument if the id is unknown.
     * @throws std::logic_error if the instance cannot be cancelled.
     */
    void cancelWorkflow(int instanceId);

    /**
     * @brief Changes a data entry of an instance. Can be undone with undoLast().
     * @param instanceId The instance id.
     * @param key The data key.
     * @param value The new value.
     * @throws std::invalid_argument if the id is unknown.
     */
    void changeData(int instanceId, const std::string& key, const std::string& value);

    /**
     * @brief Reassigns a work item to another participant. Can be undone with undoLast().
     * @param instanceId The instance id.
     * @param elementName Name of the work item.
     * @param p The new assignee.
     * @throws std::invalid_argument if the id is unknown or @p p is null.
     */
    void reassignItem(int instanceId, const std::string& elementName, Participant* p);

    /**
     * @brief Marks a work item as done and moves the workflow on.
     * @param instanceId The instance id.
     * @param elementName Name of the work item.
     * @throws std::invalid_argument if the id or item name is unknown.
     * @throws std::logic_error if the instance is not running.
     */
    void completeItem(int instanceId, const std::string& elementName);

    /**
     * @brief Records an approval decision for an approval item.
     *
     * Approving completes the item and moves the workflow on. Rejecting raises
     * an issue that goes through the escalation chain.
     * @param instanceId The instance id.
     * @param elementName Name of the approval item.
     * @param approved true to approve, false to reject.
     * @throws std::invalid_argument if the id is unknown or the item is not an approval.
     * @throws std::logic_error if the instance is not running.
     */
    void decideItem(int instanceId, const std::string& elementName, bool approved);

    /**
     * @brief Raises an issue on an instance and sends it down the escalation chain.
     * @param instanceId The instance id.
     * @param elementName Name of the item the issue belongs to.
     * @param type Issue category, e.g. "Damaged".
     * @param description What went wrong.
     * @param severity Severity level; higher values escalate further.
     * @throws std::invalid_argument if the id is unknown.
     * @throws std::logic_error if the instance is not running.
     */
    void reportIssue(int instanceId, const std::string& elementName, const std::string& type, const std::string& description, int severity);

    /**
     * @brief Undoes the last command that changed an instance.
     *
     * Prints a message and does nothing if there is nothing to undo.
     */
    void undoLast();

    /**
     * @brief Gets the state of an instance.
     * @param instanceId The instance id.
     * @return The state name, e.g. "Running".
     * @throws std::invalid_argument if the id is unknown.
     */
    std::string trackStatus(int instanceId) const;

    /**
     * @brief Lists the work items a person can still act on.
     *
     * Groups are skipped. Items that are AVAILABLE, ASSIGNED or STARTED are included.
     * @param instanceId The instance id.
     * @return The item names.
     * @throws std::invalid_argument if the id is unknown.
     */
    std::vector<std::string> getAvailableWork(int instanceId) const;

    /**
     * @brief Builds a text report for an instance.
     *
     * Lists the state, the data, each work item with its status and assignee,
     * and the audit history recorded for this instance.
     * @param instanceId The instance id.
     * @return The report text.
     * @throws std::invalid_argument if the id is unknown.
     */
    std::string generateReport(int instanceId) const;

    /**
     * @brief Registers a participant with the engine.
     * @param participant Must be heap-allocated and registered only once.
     *        The facade takes ownership and deletes it. Null is ignored.
     */
    void registerParticipant(Participant* participant);

    /**
     * @brief Adds a listener that is subscribed to every new instance.
     * @param listener Must be heap-allocated and added only once.
     *        The facade takes ownership and deletes it. Null is ignored.
     */
    void addListener(WorkflowListener* listener);
    struct ParticipantInfo {
        int id;
        std::string name;
        std::string role;
    };
    std::vector<ParticipantInfo> listParticipants() const;
    bool isApprovalItem(int instanceId, const std::string& elementName) const;
    void reassignItemToParticipant(int instanceId, const std::string& elementName, int participantId);
};

#endif