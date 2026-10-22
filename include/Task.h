#ifndef TASK_H
#define TASK_H

#include "WorkElement.h"

// Forward declarations
class Participant;
class Issue;

/**
 * @class Task
 * @brief Abstract leaf class representing single actionable work items.
 */
class Task : public WorkElement {
protected:
    Participant* assignee; /**< Assigned participant responsible for task execution. */

    /**
     * @brief Internal hook to check if prerequisites for execution are met.
     * @return true if ready, false otherwise.
     */
    virtual bool checkReady();

    /**
     * @brief Internal hook to notify listeners of state changes.
     */
    virtual void notifyListeners();

public:
    /**
     * @brief Constructor for Task.
     * @param name Name of the task.
     */
    Task(const std::string& name);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Task() {}

    void execute() override;
    void complete() override;
    bool isComplete() const override;
    void assignTo(Participant* participant) override;

    /**
     * @brief Gets the currently assigned participant.
     * @return Participant* Pointer to assigned participant.
     */
    Participant* getAssignee() const;

    /**
     * @brief Reports an execution issue for escalation.
     * @param issue The issue encountered during task execution.
     */
    void reportIssue(const Issue& issue);
};

#endif // TASK_H