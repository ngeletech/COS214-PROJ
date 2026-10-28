#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "WorkflowEngine.h"



static const int MAX_RETRIES = 3;

WorkflowEngine::WorkflowEngine(AssignmentService *assignment)
{
    this->assignment = assignment;
    this->factories = new WorkItemFactoryRegistry();
    this->nextInstanceId = 1;
}

WorkflowEngine::~WorkflowEngine()
{
    for (std::map<int, WorkflowInstance *>::iterator it = instances.begin(); it != instances.end(); ++it)
    {
        delete it->second;
    }
    for (std::map<std::string, std::vector<WorkflowDefinition *>>::iterator it = definitions.begin(); it != definitions.end(); ++it)
    {
        for (std::vector<WorkflowDefinition *>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
        {
            delete *vit;
        }
    }
    delete factories;
}

void WorkflowEngine::registerDefinition(WorkflowDefinition *definition)
{
    if (definition == nullptr)
    {
        std::cout << "Cannot register a null workflow definition." << std::endl;
        return;
    }
    std::vector<WorkItemSpec> specs = definition->getWorkItemSpecs();
    for (std::vector<WorkItemSpec>::iterator it = specs.begin(); it != specs.end(); ++it)
    {
        WorkItemSpec spec = *it;
        WorkItemFactory *factory = factories->getFactory(spec.getType());
        if (factory == nullptr)
        {
            std::cout << "No factory registered for work item type: " << spec.getType() << std::endl;
            return;
        }
    }
    std::vector<WorkflowDefinition *> &versions = definitions[definition->getName()];
    definition->setVersion(versions.size() + 1);
    versions.push_back(definition);
}
WorkflowInstance *WorkflowEngine::startInstance(std::string defName, WorkData data)
{
    std::map<std::string, std::vector<WorkflowDefinition *>>::iterator found = definitions.find(defName);
    if (found == definitions.end() || found->second.empty())
    {
        std::cout << "No workflow definition found for name: " << defName << std::endl;
        return nullptr;
    }

    WorkflowDefinition *definition = found->second[found->second.size() - 1];
    WorkflowInstance *instance = new WorkflowInstance(nextInstanceId++, definition, this, data);

    for (std::vector<WorkflowListener *>::iterator it = defaultListeners.begin(); it != defaultListeners.end(); ++it)
    {
        WorkflowListener *listener = *it;
        instance->subscribe(listener);
    }

    const std::vector<WorkItemSpec> &specs = definition->getWorkItemSpecs();
    for (std::vector<WorkItemSpec>::const_iterator it = specs.begin(); it != specs.end(); ++it)
    {
        WorkItemFactory *factory = factories->getFactory(it->getType());
        if (factory == nullptr)
        {
            std::cout << "No factory registered for work item type: " << it->getType() << std::endl;
            delete instance;
            return nullptr;
        }
        WorkElement *element = factory->create(*it, instance);
        instance->getRoot()->add(element);
    }
    instances[instance->getId()] = instance;
    instance->start();
    return instance;
}

void WorkflowEngine::completeWorkItem(int instanceId, std::string elementName)
{
    WorkflowInstance *instance = getInstance(instanceId);
    if (instance == nullptr)
    {
        std::cout << "No workflow instance found for ID: " << instanceId << std::endl;
        return;
    }

    WorkElement *element = instance->findElement(elementName);
    if (element == nullptr)
    {
        std::cout << "No work element found with name: " << elementName << " in instance ID: " << instanceId << std::endl;
        return;
    }
    element->complete();
}

void WorkflowEngine::handleIssue(Issue &issue, WorkflowInstance &instance)
{
    instance.publish(WorkflowEvent("IssueRaised", instance.getId(), issue.getElementName(), issue.getDescription()));

    EscalationHandler *chain = instance.getDefinition()->getEscalationHandler();
    if (chain == nullptr || !chain->handle(issue))
    {
        std::cout << "Issue could not be resolved: " << issue.getDescription() << std::endl;
        instance.fail(issue);
        return;
    }

    if (!issue.getElementName().empty())
    {
        WorkElement *element = instance.findElement(issue.getElementName());
        if (element != nullptr)
        {
            std::string key = "retry:" + issue.getElementName();
            std::map<std::string, std::string> current = instance.getData().getData();
            std::map<std::string, std::string>::iterator found = current.find(key);
            int tries = 0;

            if (found != current.end())
            {
                std::stringstream ss(found->second);
                ss >> tries;
            }
            if (tries >= MAX_RETRIES)
            {
                std::cout << "Giving up on " << issue.getElementName() << " after " << tries << " retries" << std::endl;
                instance.fail(issue);
                return;
            }
            std::ostringstream counter;
            counter << (tries + 1);
            instance.setData(key, counter.str());

            Participant *old = element->getAssignee();
            if (old != nullptr)
            {
                old->decrementWorkload();
                element->assignTo(nullptr);
            }
            element->setStatus(CREATED);
        }
    }

    instance.publish(WorkflowEvent("IssueResolved", instance.getId(), issue.getElementName(), issue.getDescription()));
    activateNext(instance);
}

void WorkflowEngine::assignWork(WorkflowInstance &instance, WorkflowElement *element)
{
    if (element == nullptr)
    {
        return;
    }

    assignment->setStrategy(instance.getDefinition()->getAssignmentStrategy());
    Participant *participant = assignment->assignWork(*element, instance.getData(), participants);

    if (participant == nullptr)
    {
        Issue issue("NoCandidate", "No suitable participant for " + element->getName(), 2, element->getName());
        handleIssue(issue, instance);
        return;
    }

    element->assignTo(participant);
    element->setStatus(ASSIGNED);
    participant->incrementWorkload();
    instance.publish(WorkflowEvent("ElementAssigned", instance.getId(), element->getName(), participant->getName()));
}

void WorkflowEngine::activateNext(WorkflowInstance &instance)
{
    if (instance.getState()->getName() != "Running")
    {
        return;
    }

    std::vector<WorkElement *> ready;
    AvailableItemIterator it(instance.getRoot());
    for (it.first(); !it.isDone(); it.next())
    {
        ready.push_back(it.current());
    }

    for (size_t i = 1; i < ready.size(); ++i)
    {
        WorkElement *current = ready[i];
        size_t j = i;
        while (j > 0 && ready[j - 1]->getPriority() < current->getPriority())
        {
            ready[j] = ready[j - 1];
            --j;
        }
        ready[j] = current;
    }

    for (std::vector<WorkElement *>::iterator rit = ready.begin(); rit != ready.end(); ++rit)
    {
        WorkElement *element = *rit;
        element->setStatus(AVAILABLE);

        if (element->requiresAssignee())
        {
            assignWork(instance, element);

            if (instance.getState()->getName() != "Running")
            {
                return;
            }
        }
    }
    instance.getRoot()->execute();
}

WorkflowInstance *WorkflowEngine::getInstance(int id)
{
    std::map<int, WorkflowInstance *>::iterator it = instances.find(id);
    if (it == instances.end())
    {
        return nullptr;
    }
    return it->second;
}

void WorkflowEngine::addParticipant(Participant *participant)
{
    if (participant == nullptr)
    {
        std::cout << "Cannot add a null participant" << std::endl;
        return;
    }
    participants.push_back(participant);
}
Participant *WorkflowEngine::findParticipant(int id)
{
    for (std::vector<Participant *>::iterator it = participants.begin(); it != participants.end(); ++it)
    {
        if ((*it)->getId() == id)
        {
            return *it;
        }
    }
    return nullptr;
}
void WorkflowEngine::addDefaultListener(WorkflowListener *listener)
{
    if (listener == nullptr)
    {
        std::cout << "Cannot add a null listener" << std::endl;
        return;
    }
    defaultListeners.push_back(listener);
}