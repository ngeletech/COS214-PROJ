#ifndef USERTASK_H
#define USERTASK_H

#include "Task.h"

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

#endif // USERTASK_H