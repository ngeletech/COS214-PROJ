#include "InstanceSnapshot.h"
#include "State.h"

/**
 * @brief Stores a snapshot of the instance's current state
 *        The instance builds the snapsho
 */
void RollbackManager::save(WorkflowInstance* instance){
    snapshots[instance->getId()].push_back(instance->saveSnapshot());
}

/**
 * @brief Restores the instance to its most latest restore point and discards it.
 *        Publishes "RolledBack". Does nothing if there is no snapshot
 */
void RollbackManager::rollback(WorkflowInstance* instance){
    auto it = snapshots.find(instance->getId());
    if(it == snapshots.end() || it->second.empty()){
        return;
    }
    InstanceSnapshot latest = it->second.back();
    it->second.pop_back();

    instance->restore(latest);
    instance->publish("RolledBack");
    return;
}
/**
 * @brief True if the given instance has at least one restore point.
 */
bool RollbackManager::canRollback(int instanceId) const {
    auto it = snapshots.find(instanceId);
    return it != snapshots.end() && !it->second.empty();
}