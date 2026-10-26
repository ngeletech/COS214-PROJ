#include "WorkItemFactory.h"
#include "WorkElement.h"
#include "WorkflowInstance.h"
#include "WorkItemStatus.h"
#include "UserTask.h"
#include "ServiceTask.h"
#include "ApprovalTask.h"
#include "ScriptTask.h"
#include "ParallelGroup.h"
#include "SequentialGroup.h"

/**
 * @brief Virtual destructor so concrete factories are deleted correctly through a WorkItemFactory pointer.
 */
WorkItemFactory::~WorkItemFactory() {}

/**
 * @brief Builds a work element from a spec and links it to its instance.
 *        The concrete type comes from makeElement()
 * @param spec
 * @param instance the running instance that will own the element
 * @return a new element in created status (caller takes ownership)
 */
WorkElement* WorkItemFactory::create(const WorkItemSpec& spec, WorkflowInstance* instance) {
    WorkElement* element = makeElement(spec);
    element->setInstance(instance);
    element->setStatus(CREATED);
    
    return element;
}

//ConcreteCreators

/**
 * @brief Creates a UserTask
 */
WorkElement* UserTaskFactory::makeElement(const WorkItemSpec& spec){
    return new UserTask(spec.getName());
}

/**
 * @brief Creates a ServiceTask
 */
WorkElement* ServiceTaskFactory::makeElement(const WorkItemSpec& spec){
    return ServiceTask(spec.getName());
}

/**
 * @brief Creates an ApprovalTask
 */
WorkElement* ApprovalTaskFactory::makeElement(const WorkItemSpec& spec){
    return new ApprovalTask(spec.getName());
}


/**
 * @brief Creates a ScripTask that writes spec.key = spec.value into the data
 */
WorkElement* ScriptTaskFactory::makeElement(const WorkItemSpec& spec){
    auto config = spec.getConfig().getData();
    auto key = config.find("key");
    auto value = config.find("value");
    if(key == config.end() || value == config.end()){
        throw TividyError();
    }
    return new ScriptTask(spec.getName(), key->second, spec.value);
}

/**
 * @brief Creates a ParallelGroup
 */
WorkElement* ParallelGroupFactory::makeElement(const WorkItemSpec& spec){
    return new ParallelGroup(spec.getName());
}

/**
 * @brief
 */
WorkElement* SequentialGroupFactory::makeElement(const WorkItemSpec& spec){
    return new SequentialGroup(spec.getName());
}

//WorkItemFactoryRegistry

/**
 * @brief Creates an empty registry
 */
WorkItemFactoryRegistry::WorkItemFactoryRegistry(){
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

    return it->second
    
}