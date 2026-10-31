#ifndef TEMPLATE_METHOD_H
#define TEMPLATE_METHOD_H

#include <string>
#include "composite.h"

/**
 * @class UserTask
 * @brief Concrete task requiring human participant interaction.
 */
class UserTask : public Task {
protected:
    /**
     * @brief Primitive step overriding doWork to handle user input/interaction.
     */
    void doWork() override;

public:
    /**
     * @brief Constructor for UserTask.
     * @param name Name of the user task.
     */
    UserTask(const std::string& name);

    virtual ~UserTask() {}
};

/**
 * @class ServiceTask
 * @brief Concrete task executing automated system operations via external connectors.
 */
class ServiceTask : public Task {
protected:
    /**
     * @brief Primitive step overriding doWork to invoke external service APIs.
     */
    void doWork() override;

public:
    /**
     * @brief Constructor for ServiceTask.
     * @param name Name of the service task.
     */
    ServiceTask(const std::string& name);

    virtual ~ServiceTask() {}
    bool requiresAssignee() const override;
};

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

/**
 * @class ScriptTask
 * @brief Concrete task executing scripted operations and data mutations.
 */
class ScriptTask : public Task {
private:
    std::string key;   /**< Target data key. */
    std::string value; /**< Target data value. */

protected:
    /**
     * @brief Primitive step overriding doWork to execute internal scripts.
     */
    void doWork() override;

public:
    /**
     * @brief Constructor for ScriptTask.
     * @param name Name of the script task.
     * @param key Script parameter key.
     * @param value Script parameter value.
     */
    ScriptTask(const std::string& name, const std::string& key, const std::string& value);

    virtual ~ScriptTask() {}
    bool requiresAssignee() const override;
};

#endif // TEMPLATE_METHOD_H