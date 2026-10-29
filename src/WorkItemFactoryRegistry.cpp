#include "WorkItemFactory.h"


/**
 * @brief Creates an empty registry
 */
WorkItemFactoryRegistry::WorkItemFactoryRegistry(){
    registerFactory("UserTask", new UserTaskFactory());
    registerFactory("ServiceTask", new ServiceTaskFactory());
    registerFactory("ApprovalTask", new ApprovalTaskFactory());
    registerFactory("ScriptTask", new ScriptTaskFactory());
    registerFactory("ParallelGroup", new ParallelGroupFactory());
    registerFactory("SequentialGroup", new SequentialGroupFactory());
}

/**
 * @brief Deletes every factory the registry owns
 */
WorkItemFactoryRegistry::~WorkItemFactoryRegistry(){
    for (auto&  factory : factories){
        delete factory.second;
    }
    
}

/**
 * @brief Registers a factory for a spec type. Takes ownership.
 *        Replaces (and deletes) any factory already registered for that type.
 */
void WorkItemFactoryRegistry::registerFactory(const std::string type, WorkItemFactory* factory) {
    auto it = factories.find(type);
    if(it != factories.end()){
        if (it->second != factory) {
            delete it->second;
            it->second = factory;
        }
    } else {
        factories[type] = factory;
    }
}

/**
 * @brief Finds the factory for a spec type.
 * @return the factory, or nullptr if none is registered 
 */

WorkItemFactory* WorkItemFactoryRegistry::getFactory(const std::string& type) {
    auto it = factories.find(type);
    if (it == factories.end()){
        return nullptr;
    }

    return it->second;
    
}