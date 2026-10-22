#ifndef SERVICETASK_H
#define SERVICETASK_H

#include "Task.h"

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
};

#endif // SERVICETASK_H