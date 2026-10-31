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
#include "WorkflowEvent.h"

class WorkflowDefinition;
class Issue;
class WorkGroup;
class WorkElement;
class WorkflowEngine;
class WorkflowListener;
class InstanceSnapshot;
class WorkflowInstance;

// ---------------- State pattern ----------------

/**
 * @class InstanceState
 * @brief Abstract State. Declares every lifecycle action.
 *
 * Every action throws std::logic_error by default. A concrete state
 * overrides only the actions that are legal while in that state.
 */
class InstanceState {
protected:
    /** @brief The context this state controls (not owned). */
    WorkflowInstance* instance;

public:
    /**
     * @brief Binds the state to its context.
     * @param i The WorkflowInstance this state controls.
     */
    InstanceState(WorkflowInstance& i);

    /** @brief Virtual destructor so states can be deleted through an InstanceState pointer. */
    virtual ~InstanceState() {}

    /**
     * @brief Moves Created -> Running.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void start();

    /**
     * @brief Moves Running -> Suspended.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void suspend();

    /**
     * @brief Moves Suspended -> Running.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void resume();

    /**
     * @brief Moves Running -> Completed.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void complete();

    /**
     * @brief Moves to Cancelled and cancels outstanding work.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void cancel();

    /**
     * @brief Records a failure and moves to Failed.
     * @param issue The problem that caused the failure.
     * @throws std::logic_error if not allowed in the current state.
     */
    virtual void fail(const Issue& issue);

    /**
     * @brief Checks whether cancel() is allowed in this state.
     * @return false by default; true in Created, Running and Suspended.
     */
    virtual bool canCancel() const;

    /**
     * @brief Gets the state name, used by the Memento to save and restore it.
     * @return The state name, e.g. "Created" or "Running".
     */
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

/**
 * @class CreatedState
 * @brief ConcreteState. A new instance that has not started yet.
 *
 * Allows start() and cancel().
 */
class CreatedState : public InstanceState {
public:
    /**
     * @brief Binds the state to its context.
     * @param i The context.
     */
    CreatedState(WorkflowInstance& i);

    /** @brief Moves the instance to Running. */
    void start() override;

    /** @brief Moves the instance to Cancelled. */
    void cancel() override;

    /** @brief Cancelling is allowed in this state. @return true. */
    bool canCancel() const override;

    /** @brief Gets the state name. @return "Created". */
    std::string getName() const override;
};

/**
 * @class RunningState
 * @brief ConcreteState. The instance is executing.
 *
 * Allows suspend(), complete(), cancel() and fail().
 */
class RunningState : public InstanceState {
public:
    /**
     * @brief Binds the state to its context.
     * @param i The context.
     */
    RunningState(WorkflowInstance& i);

    /** @brief Moves the instance to Suspended. */
    void suspend() override;

    /** @brief Moves the instance to Completed. */
    void complete() override;

    /** @brief Moves the instance to Cancelled. */
    void cancel() override;

    /**
     * @brief Moves the instance to Failed.
     * @param issue The problem that caused the failure.
     */
    void fail(const Issue& issue) override;

    /** @brief Cancelling is allowed in this state. @return true. */
    bool canCancel() const override;

    /** @brief Gets the state name. @return "Running". */
    std::string getName() const override;
};

/**
 * @class SuspendedState
 * @brief ConcreteState. The instance is paused.
 *
 * Allows resume() and cancel().
 */
class SuspendedState : public InstanceState {
public:
    /**
     * @brief Binds the state to its context.
     * @param i The context.
     */
    SuspendedState(WorkflowInstance& i);

    /** @brief Moves the instance back to Running. */
    void resume() override;

    /** @brief Moves the instance to Cancelled. */
    void cancel() override;

    /** @brief Cancelling is allowed in this state. @return true. */
    bool canCancel() const override;

    /** @brief Gets the state name. @return "Suspended". */
    std::string getName() const override;
};

/**
 * @class CompletedState
 * @brief ConcreteState. Finished successfully. Terminal: every action is refused.
 */
class CompletedState : public InstanceState {
public:
    /**
     * @brief Binds the state to its context.
     * @param i The context.
     */
    CompletedState(WorkflowInstance& i);

    /** @brief Gets the state name. @return "Completed". */
    std::string getName() const override;
};

/**
 * @class CancelledState
 * @brief ConcreteState. Cancelled by a user. Terminal: every action is refused.
 */
class CancelledState : public InstanceState {
public:
    /**
     * @brief Binds the state to its context.
     * @param i The context.
     */
    CancelledState(WorkflowInstance& i);

    /** @brief Gets the state name. @return "Cancelled". */
    std::string getName() const override;
};

/**
 * @class FailedState
 * @brief ConcreteState. Ended with an error. Terminal: every action is refused.
 */
class FailedState : public InstanceState {
public:
    /**
     * @brief Binds the state to its context.
     * @param i The context.
     */
    FailedState(WorkflowInstance& i);

    /** @brief Gets the state name. @return "Failed". */
    std::string getName() const override;
};

// ---------------- Context ----------------

/**
 * @class WorkflowInstance
 * @brief Context. One running copy of a workflow.
 *
 * Forwards lifecycle calls to its current InstanceState. It owns the current
 * state and the root WorkGroup, and deletes both in its destructor.
 *
 * Also acts as the Observer Subject (subscribe/publish) and the Memento
 * Originator (saveSnapshot/restore).
 */
class WorkflowInstance {
private:
    int id;                                    ///< Unique instance id.
    WorkData data;                             ///< Key/value data for this run.
    InstanceState* state;                      ///< Current state (owned).
    const WorkflowDefinition* definition;      ///< Definition this was made from (not owned).
    WorkGroup* root;                           ///< Root of the work tree (owned).
    WorkflowEngine* engine;                    ///< Engine used to look up participants (not owned).
    std::vector<WorkflowListener*> listeners;  ///< Subscribed observers (not owned).
    std::vector<WorkflowEvent> pending;
    bool publishing;

public:
    /**
     * @brief Creates an instance in the Created state.
     * @param id Unique instance id.
     * @param definition Workflow definition (not owned).
     * @param data Initial data.
     * @param engine Engine used to look up participants (not owned, may be null).
     */
    WorkflowInstance(int id, const WorkflowDefinition* definition, WorkData data, WorkflowEngine* engine);

    /** @brief Deletes the current state and the root WorkGroup. */
    ~WorkflowInstance();

    /**
     * @brief Gets the instance id.
     * @return The id.
     */
    int getId() const;

    /**
     * @brief Gets the definition this instance was created from.
     * @return The definition (not owned), or nullptr if none was given.
     */
    const WorkflowDefinition* getDefinition() const;

    /**
     * @brief Replaces the root, deleting the old one.
     * @param newRoot The new root. The instance takes ownership.
     */
    void setRoot(WorkGroup* newRoot);

    /**
     * @brief Gets the root of the work tree.
     * @return The root, or nullptr if none has been set.
     */
    WorkGroup* getRoot() const;

    /**
     * @brief Gets the name of the current state.
     * @return e.g. "Created" or "Running".
     */
    std::string getStateName() const;

    /**
     * @brief gets the engine
     * @return the enginr
     */
    WorkflowEngine* getEngine() const;

    /**
     * @brief Starts the instance.
     * @throws std::logic_error if the current state refuses.
     */
    void start();

    /**
     * @brief Suspends the instance.
     * @throws std::logic_error if the current state refuses.
     */
    void suspend();

    /**
     * @brief Resumes the instance.
     * @throws std::logic_error if the current state refuses.
     */
    void resume();

    /**
     * @brief Completes the instance.
     * @throws std::logic_error if the current state refuses.
     */
    void complete();

    /**
     * @brief Cancels the instance.
     * @throws std::logic_error if the current state refuses.
     */
    void cancel();

    /**
     * @brief Fails the instance.
     * @param issue The problem that caused the failure.
     * @throws std::logic_error if the current state refuses.
     */
    void fail(const Issue& issue);

    /**
     * @brief Switches state and deletes the previous one.
     *
     * Called by the concrete states to make transitions.
     * @param newState The new state. The instance takes ownership.
     */
    void setState(InstanceState* newState);

    /**
     * @brief Observer: registers a listener.
     * @param listener The listener (not owned). Null pointers are ignored.
     */
    void subscribe(WorkflowListener* listener);

    /**
     * @brief Observer: notifies every listener of an event.
     * @param event Event name, e.g. "DataChanged".
     */
    //void publish(const std::string& event);
    void publish(const std::string& event, const std::string& elementName = "");

    /**
     * @brief Memento: captures data, state name, element statuses and assignments.
     * @return The snapshot.
     */
    InstanceSnapshot saveSnapshot();

    /**
     * @brief Memento: restores a previously saved snapshot.
     * @param snapshot A snapshot from saveSnapshot().
     */
    void restore(const InstanceSnapshot& snapshot);

    /**
     * @brief Gets the current data.
     * @return The data.
     */
    const WorkData& getData() const;

    /**
     * @brief Adds or updates a data entry and publishes "DataChanged".
     * @param key The data key.
     * @param value The new value.
     */
    void setData(const std::string& key, const std::string& value);

    /**
     * @brief Finds a work element in the tree by name.
     * @param name Name of the element to look for.
     * @return The first matching element, or nullptr if there is no root or no match.
     */
    WorkElement* findElement(const std::string& name);

    /**
     * @brief Sets every unfinished element to CANCELLED.
     *
     * COMPLETED, REJECTED and already CANCELLED elements are left unchanged.
     * Called by the states when the instance is cancelled.
     */
    void cancelOutstandingWork();
};

#endif