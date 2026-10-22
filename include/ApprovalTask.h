#ifndef APPROVALTASK_H
#define APPROVALTASK_H

#include "Task.h"

/**
 * @class ApprovalTask
 * @brief Concrete task managing decision-based approval workflows.
 */
class ApprovalTask : public Task {
private:
    bool approved; /**< Approval decision result. */

protected:
    /**
     * @brief Primitive step overriding doWork to process approval decisions.
     */
    void doWork() override;

public:
    /**
     * @brief Constructor for ApprovalTask.
     * @param name Name of the approval task.
     */
    ApprovalTask(const std::string& name);

    virtual ~ApprovalTask() {}

    /**
     * @brief Records an explicit approval or rejection decision.
     * @param approved True if approved, false if rejected.
     */
    void recordDecision(bool approved);
};

#endif // APPROVALTASK_H