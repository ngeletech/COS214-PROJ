#ifndef WORKITEMFACTORY_H
#define WORKITEMFACTORY_H

#include<map>
#include <string>

class WorkElement;
class WorkflowInstance;
class WorkItemSpec;

/**
 * @brief Creator in the Factory Method pattern.
 * create() is shared
 * makeElement() is the factory method that each subclass overrides
 */

class WorkItemFactory {
    public:
        /**
         * @brief Virtual destructor so factories can be deleted through a WorkItemFactory pointer
         */
        virtual ~WorkItemFactory();

        /**
         * @brief Builds a work element from a spec and links it to its instance.
         * @param spec the work item description (type, name and config)
         * @param instance the running instance the element belongs to
         * @return a new element with status CREATED, the caller takes ownership
         */
        WorkElement* create(const WorkItemSpec& spec, WorkflowInstance* instance);
    
    protected:
        /**
         * @brief Factory Method: creates the concrete element for a spec
         * @param spec work item description
         * @return a new element 
         */
        virtual WorkElement* makeElement(const WorkItemSpec& spec) = 0;
};

/**
 * @brief Concrete Creator that builds a User Task
 */
class UserTaskFactory : public WorkItemFactory {
    protected:
        /**
         * @brief Creates a UserTask named after the spec.
         * @param spec the work item description
         * @return a new UserTask
         */
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};


/**
 * @brief Concrete Creator that builds a Service Task
 */
class ServiceTaskFactory : public WorkItemFactory {
    protected:
        /**
         * @brief Creates a ServiceTask named after the spec.
         * @param spec the work item description
         * @return a new ServiceTask
         */
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that builds an Approval Task
 */
class ApprovalTaskFactory : public WorkItemFactory {
    protected:
        /**
         * @brief Creates a ApprovalTask named after the spec.
         * @param spec the work item description
         * @return a new ApprovalTask
         */

        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that builds a Script Task
 */
class ScriptTaskFactory : public WorkItemFactory {
    protected:
        /**
         * @brief Creates a ScriptTask that writes config "key" = config "value" into the data
         * @param spec the work item description; its config must contain "key" and "value"
         * @return a new ScriptTask
         * @throws TividyError if "key" or "value" is missing from the config
         */
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that builds a ParallelGroup
 */
class ParallelGroupFactory : public WorkItemFactory {
    protected:
        /**
         * @brief Creates an empty ParallelGroup named after the spec
         * @param spec the work item description
         * @return a new ParallelGroup
         */
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that builds a SequentialGroup
 */
class SequentialGroupFactory : public WorkItemFactory {
    protected:
        /**
         * @brief Creates an empty SequentialGroup named after the spec
         * @param spec the work item description
         * @return a new SequentialGroup
         */
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};


 /**
  * @class WorkItemFactoryRegistry
  * @brief Lets the engine find the right factory for a spec's type
  */
class WorkItemFactoryRegistry {
    public:
        /**
         * @brief Creates a registry with the six standard factories already registered
         */
        WorkItemFactoryRegistry();

        /**
         * @brief Deletes every factory registry owns
         */
        ~WorkItemFactoryRegistry();

        /**
         * @brief Registers a factory for a spec type and takes ownership of it
         * A factory already registered under the same type is deleted and replaced
         * @param type the spec type
         * @param factory the factory to register
         */
        void registerFactory(const std::string type, WorkItemFactory* factory);

        /**
         * @brief Finds the factory for a spec type
         * @param type the spec type
         * @return the factory or null ptr if none is registered
         */
        WorkItemFactory* getFactory(const std::string& type);

    private:
        std::map<std::string, WorkItemFactory*> factories;
};
#endif