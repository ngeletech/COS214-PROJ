#include "WorkItemFactory.h"
#include "State.h"
#include "composite.h"
#include "Builder.h"
#include "WorkData.h"
#include "TemplateMethod.h"
#include <stdexcept>


namespace {
    /**
     * @brief Removes the leading and trailing spaces and tabs from a string*/
    std::string trim(const std::string& text) {
        const char* whitespace = " \t";
        std::string::size_type first = text.find_first_not_of(whitespace);
        if (first == std::string::npos) {
            return "";
        }
        std::string::size_type last = text.find_last_not_of(whitespace);
        return text.substr(first, last - first + 1);
        
    }
}
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
    element->setStatus(WorkItemStatus::CREATED);
    
    return element;
}

//ConcreteCreators

/**
 * @brief Creates a UserTask
 */
WorkElement* UserTaskFactory::makeElement(const WorkItemSpec& spec){
    return new UserTask(spec.name);
}

/**
 * @brief Creates a ServiceTask
 */
WorkElement* ServiceTaskFactory::makeElement(const WorkItemSpec& spec){
    return new ServiceTask(spec.name);
}

/**
 * @brief Creates an ApprovalTask
 */
WorkElement* ApprovalTaskFactory::makeElement(const WorkItemSpec& spec){
    return new ApprovalTask(spec.name);
}


/**
 * @brief The specs config is a single string for a script task it is assumed to look like "key,value"; only the first "," splits it. 
 */
WorkElement* ScriptTaskFactory::makeElement(const WorkItemSpec& spec){
    std::string::size_type comma = spec.config.find(',');
    if(comma == std::string::npos){
       throw std::invalid_argument("ScriptTask config must look like \"key,value\"");
    }
    std::string key = trim(spec.config.substr(0, comma));
    std::string value = trim(spec.config.substr(comma + 1));
    
    if(key.empty()) {
        throw std::invalid_argument("ScriptTask config needs a non-empty key");
    }
    return new ScriptTask(spec.name, key, value);
}

/**
 * @brief Creates a ParallelGroup
 */
WorkElement* ParallelGroupFactory::makeElement(const WorkItemSpec& spec){
    return new ParallelGroup(spec.name);
}

/**
 * @brief
 */
WorkElement* SequentialGroupFactory::makeElement(const WorkItemSpec& spec){
    return new SequentialGroup(spec.name);
}

