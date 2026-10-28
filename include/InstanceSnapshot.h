#ifndef INSTANCESNAPSHOT_H
#define INSTANCESNAPSHOT_H

#include <map>
#include <string>
#include <vector>
#include "WorkData.h"
#include "composite.h"

class WorkflowInstance;

/**
 * @class InstanceSnapshot
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
 * @class RollbackManager
 * @brief Caretaker. Stores the snapshots per instance without looking inside them
 * Snapshots are kept separately for each instance id, so rolling back one order
 * can never restore another order's state
 */

class RollbackManager {
    public:
        /**
         * @brief saves a snapshot of the instance's current state
         * @param instance the instance to save (the instance builds the snapshot itself)
         */
        void save(WorkflowInstance* instance);

        /**
         * @brief Restores the instance to its most recent snapshot and discards the snapshot
         * @param instance the instance to restore
         */
        void rollback(WorkflowInstance* instance);

        /**
         * @brief Checks whether an instance has a snapshot to roll back to.
         * @param instanceId the id of the instance
         * @return true if at least one snapshot saved for that instance 
         */
        bool canRollback(int instanceId) const ;

    private:
        std::map<int, std::vector<InstanceSnapshot>> snapshots;//save snapshots per instance
};

#endif