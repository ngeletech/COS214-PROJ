/**
 * @file State.h
 * @brief State pattern for the workflow instance lifecycle.
 *
 * Pattern roles:
 *  - Context: WorkflowInstance
 *  - State: InstanceState
 *  - ConcreteState: CreatedState, RunningState, SuspendedState,
 *                   CompletedState, CancelledState, FailedState
 *
 * WorkflowInstance also takes part in the Observer pattern (as Subject) and
 * the Memento pattern (as Originator).
 */
#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>
#include "WorkData.h"

class WorkflowDefinition;
class Issue;
class WorkGroup;
class WorkflowEngine;
class WorkflowListener;
class InstanceSnapshot;
class WorkflowInstance;

// ---------------- State pattern ----------------

/**
 * @class InstanceState
 * @brief Abstract State: declares every lifecycle action.
 *
 * Every action throws std::logic_error by default. A concrete state
 * overrides only the actions that are legal while in that state.
 */
class InstanceState {
protected:
    WorkflowInstance* instance; 
public:
    /// @brief Binds the state to its context.
    /// @param i The WorkflowInstance this state controls.
    InstanceState(WorkflowInstance& i);

    virtual ~InstanceState() {}

    /// @brief Moves Created -> Running.
    /// @throws std::logic_error if not allowed in the current state.
    virtual void start();

    /// @brief Moves Running -> Suspended.
    /// @throws std::logic_error if not allowed in the current state.
    virtual void suspend();

    /// @brief Moves Suspended -> Running.
    /// @throws std::logic_error if not allowed in the current state.
    virtual void resume();

    /// @brief Moves Running -> Completed.
    /// @throws std::logic_error if not allowed in the current state.
    virtual void complete();

    /// @brief Moves to Cancelled, where cancelling is allowed.
    /// @throws std::logic_error if not allowed in the current state.
    virtual void cancel();

    /**
     * @brief Records a failure and moves to Failed.
     * @param issue The problem that caused the failure.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void fail(const Issue& issue);

    /// @brief Whether cancel() is allowed in this state.
    /// @return false by default.
    virtual bool canCancel() const;

    /// @brief Name of the state, used by the Memento to save and restore it.
    /// @return e.g. "Created", "Running".
    virtual std::string getName() const = 0;

    /**
     * @brief Factory used when restoring from a snapshot.
     * @param name Value previously returned by getName().
     * @param inst Context the new state will belong to.
     * @return A newly allocated state; the caller takes ownership.
     * @throws std::invalid_argument if @p name is unknown.
     */
    static InstanceState* create(const std::string& name, WorkflowInstance& inst);
};

/// @brief ConcreteState: a new instance. Allows start() and cancel().
class CreatedState : public InstanceState {
public:
    CreatedState(WorkflowInstance& i);
    void start() override;
    void cancel() override;
    bool canCancel() const override;
    std::string getName() const override;
};

/// @brief ConcreteState: executing. Allows suspend(), complete(), cancel() and fail().
class RunningState : public InstanceState {
public:
    RunningState(WorkflowInstance& i);
    void suspend() override;
    void complete() override;
    void cancel() override;
    void fail(const Issue& issue) override;
    bool canCancel() const override;
    std::string getName() const override;
};

/// @brief ConcreteState: paused. Allows resume() and cancel().
class SuspendedState : public InstanceState {
public:
    SuspendedState(WorkflowInstance& i);
    void resume() override;
    void cancel() override;
    bool canCancel() const override;
    std::string getName() const override;
};

/// @brief ConcreteState: finished successfully. Terminal; no actions allowed.
class CompletedState : public InstanceState {
public:
    CompletedState(WorkflowInstance& i);
    std::string getName() const override;
};

/// @brief ConcreteState: cancelled by a user. Terminal; no actions allowed.
class CancelledState : public InstanceState {
public:
    CancelledState(WorkflowInstance& i);
    std::string getName() const override;
};

/// @brief ConcreteState: ended with an error. Terminal; no actions allowed.
class FailedState : public InstanceState {
public:
    FailedState(WorkflowInstance& i);
    std::string getName() const override;
};

// ---------------- Context ----------------

/**
 * @class WorkflowInstance
 * @brief Context: one running copy of a workflow.
 *
 * Forwards lifecycle calls to its current InstanceState. It owns the current
 * state and the root WorkGroup, and deletes both in its destructor.
 *
 * Also acts as the Observer Subject (subscribe/publish) and the Memento
 * Originator (saveSnapshot/restore).
 */
class WorkflowInstance {
private:
    int id;                                   
    WorkData data;                             
    InstanceState* state;                     
    const WorkflowDefinition* definition;      
    WorkGroup* root;                           
    WorkflowEngine* engine;                    
    std::vector<WorkflowListener*> listeners;  
public:
    /**
     * @brief Creates an instance in the Created state.
     * @param id Unique instance id.
     * @param definition Workflow definition (not owned).
     * @param data Initial data.
     * @param engine Engine used to look up participants (not owned).
     */
    WorkflowInstance(int id, const WorkflowDefinition*, WorkData data, WorkflowEngine* engine);

    /// @brief Deletes the current state and the root WorkGroup.
    ~WorkflowInstance();

    /// @brief Instance id.
    int getId() const;

    /// @brief Definition this instance was created from.
    const WorkflowDefinition* getDefinition() const;

    /// @brief Replaces the root, deleting the old one. Takes ownership of @p newRoot.
    void setRoot(WorkGroup* newRoot);

    /// @brief Root of the work tree.
    WorkGroup* getRoot() const;

    /// @brief Name of the current state.
    std::string getStateName() const;

    /// @name Lifecycle (delegated to the current state)
    /// @throws std::logic_error if the action is not allowed in the current state.
    ///@{
    void start();
    void suspend();
    void resume();
    void complete();
    void cancel();
    /// @param issue The problem that caused the failure.
    void fail(const Issue& issue);
    ///@}

    /// @brief Switches state and deletes the previous one. Takes ownership of @p newState.
    /// Called by the concrete states to make transitions.
    void setState(InstanceState* newState);

    /// @brief Observer: registers a listener. Null pointers are ignored.
    void subscribe(WorkflowListener* listener);

    /// @brief Observer: notifies every listener of an event.
    /// @param event Event name, e.g. "DataChanged".
    void publish(const std::string& event);

    /// @brief Memento: captures data, state name, element statuses and assignments.
    InstanceSnapshot saveSnapshot();

    /// @brief Memento: restores a previously saved snapshot.
    /// @param snapshot Snapshot from saveSnapshot().
    void restore(const InstanceSnapshot& snapshot);

    /// @brief Current data.
    const WorkData& getData() const;

    /// @brief Adds or updates a data entry and publishes "DataChanged".
    void setData(const std::string& key, const std::string& value);
};

#endif