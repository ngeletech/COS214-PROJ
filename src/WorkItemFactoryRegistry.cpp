#include "WorkItemFactory.h"


/**
 * @brief Registers the six standard factories under "UserTask", "ServiceTask"
 *        "ApprovalTask", "ScriptTask", "ParallelGroup" and "SequentialGroup"
 *         Without this the engine would have nothing to look up
 * @details uses WorkItemFactoryRegistry registerFactory(name, Factory)
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
 * @details If the type is alread registered the old factory is deleted and replaced.
 *          Registering the same pointer twice does nothing so the factory is never
 *          deleted while it is still in use
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
 * @details Uses find() so that an unknown type does not create an
 *          empty entry
 * @return the factory, or nullptr if none is registered 
 */

WorkItemFactory* WorkItemFactoryRegistry::getFactory(const std::string& type) {
    auto it = factories.find(type);
    if (it == factories.end()){
        return nullptr;
    }

    return it->second;
    
}