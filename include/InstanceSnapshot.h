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
 * RollbackManager can only store and return it.
 */
class InstanceSnapshot {
    friend class WorkflowInstance;

    private:
        InstanceSnapshot() {}
    private:
        WorkData data;
        std::string stateName;
        std::map<std::string, WorkItemStatus> elementStatuses;
        std::map<std::string, int> assignments;

};

/**
 * @brief Caretaker. Stores the snapshots per instance without looking inside them
 */

class RollbackManager {
    public:
        void saveRestorePoint(WorkflowInstance* instance);
        void rollback(WorkflowInstance& instance);
        bool canRollback(int instanceId);

    private:
        std::map<int, std::vector<InstanceSnapshot>> snapshots;//save snapshots per instance
};

#endif