#ifndef SCRIPTTASK_H
#define SCRIPTTASK_H

#include "Task.h"

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
};

#endif // SCRIPTTASK_H