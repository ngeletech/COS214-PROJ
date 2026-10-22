#include "Memento.h"

/**
 * @brief Stores a snapshot of the instance's current state
 *        The instance builds the snapsho
 */
void RollbackManager::save(WorkflowInstance* instance){
    snapshots[instance.getId()].push_back(instance->saveSnapshot());
}

/**
 * @brief Restores the instance to its most recent snapshot and discards it.
 *        Does nothing if the instance has no snapshot.
 */
void RollbackManager::rollback(WorkflowInstance* instance){
    auto it = snapshots.find(instance->getId());
    if(it == snapshots.end() || it->second.empty()){
        return;
    }
    instance->restore(it->second.back());
    it->second.pop_back();
}