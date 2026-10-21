#ifndef FACTORYMETHOD_H
#define FACTORYMETHOD_H

#include<map>
#include <string>

class WorkElement;
class WorkflowInstance;
class WorkItemSpec;

/**
 * @brief Creator in the Factory Method pattern.
 */

class WorkItemFactory {
    public:
        virtual ~WorkItemFactory();

        WorkElement* create(const WorkItemSpec& spec, WorkflowInstance* instance);
        /**@brief */
        virtual WorkElement* makeElement(const WorkItemSpec& spec) = 0;
};

/**
 * @brief Concrete Creator that creates a User Task
 */
class UserTaskFactory : public WorkItemFactory {
    protected:
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};


/**
 * @brief Concrete Creator that creates a Service Task
 */
class ServiceTaskFactory : public WorkItemFactory {
    protected:
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that creates a Approval Task
 */
class ApprovalTaskFactory : public WorkItemFactory {
    protected:
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that creates a Script Task
 */
class ScriptTaskFactory : public WorkItemFactory {
    protected:
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that creates a ParallelGroup
 */
class ParallelGroupFactory : public WorkItemFactory {
    protected:
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};

/**
 * @brief Concrete Creator that creates a SequentialGroup
 */
class SequentialGroupFactory : public WorkItemFactory {
    protected:
        WorkElement* makeElement(const WorkItemSpec& spec) override;
};


/**
 * @brief lets the engine fin the right factory for a specs type
 */

class WorkItemFactoryRegistry {
    public:
        WorkItemFactoryRegistry();
        ~WorkItemFactoryRegistry();

        /**
         * @brief registers factorys
         */
        void registerFactory(const std::string type, WorkItemFactory* factory);

        /**
         * @brief returns factory of type type
         */
        WorkItemFactory* getFactory(const std::string& type);

    private:
        std::map<std::string, WorkItemFactory*> factories;
};
#endif