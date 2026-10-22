#ifndef MEMENTO_H
#define MEMENTO_H

#include <map>
#include <string>
#include <vector>
#include "WorkData.h"
#include "WorkItemStatus.h"

class WorkflowInstance;

/**
 * @brief Memento. Only WorkflowInstance (the Originator) can create it or read its contents;
 */
class InstanceSnapshot {
    private:
        WorkData data;
        std::string stateName;
        std::map<std::string, WorkItemStatus> elementStatuses;
        std::map<std::string, int> assignments;

};

/**
 * @brief Caretaker. Stores the snapshots per instance
 */

class RollbackManager {
    public:
        void save(WorkflowInstance* instance);
        void rollback(WorkflowInstance* instance);

    private:
        std::vector<InstanceSnapshot> snapshots;
};

#endif