#include "InstanceSnapshot.h"

/**
 * @brief Stores a snapshot of the instance's current state
 *        The instance builds the snapsho
 */
void RollbackManager::saveRestorePoint(WorkflowInstance& instance){
    snapshots[instance.getId()].push_back(instance.createSnapshot());
}

/**
 * @brief Restores the instance to its most latest restore point and discards it.
 *        Publishes "RolledBack". The audit log is not rewound
 * @return false if the instance has no restore point, otherwise true
 */
/* void RollbackManager::rollback(WorkflowInstance* instance){
     auto it = snapshots.find(instance->getId());
     if(it == snapshots.end() || it->second.empty()){
         return;
     }
     instance->restore(it->second.back());
     it->second.pop_back();
 }
*/
bool RollbackManager::rollback(WorkflowInstance& instance){
    auto it = snapshots.find(instance.getId());
    if(it == snapshots.end() || it->second.empty()){
        return false;
    }
    InstanceSapshot latest = it->second.back();
    it->second.pop_back();

    instance(latest);
    instance.publish(WorkflowEvent("RolledBack", instance.getId()))
    return true;
}
/**
 * @brief True if the given instance has at least one restore point.
 */
bool RollbackManager::canRollback(int instanceId){
    auto it = snapshots.find(instanceId);
    return it != snapshots.end() && !it->second.empty();
}