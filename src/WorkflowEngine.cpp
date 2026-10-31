#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "WorkflowEngine.h"
#include "WorkItemFactory.h"
#include "State.h"
#include "Iterator.h"
#include "Participant.h"
#include "Issue.h"
#include "Builder.h"
#include "Chain-Of-Responsibility.h"
#include "composite.h"
#include "WorkflowEvent.h"




static const int MAX_RETRIES = 3;

WorkflowEngine::WorkflowEngine(AssignmentService *assignment, WorkItemFactoryRegistry* factories)
{
    this->assignment = assignment;
    this->factories = factories;
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
        WorkItemFactory *factory = factories->getFactory(spec.type);
        if (factory == nullptr)
        {
            std::cout << "No factory registered for work item type: " << spec.type << std::endl;
            return;
        }
    }
    std::vector<WorkflowDefinition *> &versions = definitions[definition->getName()];
    //definition->setVersion(versions.size() + 1);
    versions.push_back(definition);
}
WorkflowInstance *WorkflowEngine::startInstance(const std::string& defName, WorkData data)
{
    std::map<std::string, std::vector<WorkflowDefinition *>>::iterator found = definitions.find(defName);
    if (found == definitions.end() || found->second.empty())
    {
        std::cout << "No workflow definition found for name: " << defName << std::endl;
        return nullptr;
    }

    WorkflowDefinition *definition = found->second[found->second.size() - 1];
    WorkflowInstance *instance = new WorkflowInstance(nextInstanceId++, definition,data, this);
    
    SequentialGroup* root = new SequentialGroup(definition->getName());
    root->setInstance(instance);      // so children added later inherit the instance
    instance->setRoot(root);
    for (std::vector<WorkflowListener *>::iterator it = defaultListeners.begin(); it != defaultListeners.end(); ++it)
    {
        WorkflowListener *listener = *it;
        instance->subscribe(listener);
    }

    const std::vector<WorkItemSpec> &specs = definition->getWorkItemSpecs();
    for (std::vector<WorkItemSpec>::const_iterator it = specs.begin(); it != specs.end(); ++it)
    {
        WorkItemFactory *factory = factories->getFactory(it->type);
        if (factory == nullptr)
        {
            std::cout << "No factory registered for work item type: " << it->type << std::endl;
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

void WorkflowEngine::completeWorkItem(int instanceId, const std::string& elementName)
{
    WorkflowInstance *instance = getInstance(instanceId);
    if (instance == nullptr)
    {
        std::cout << "No workflow instance found for ID: " << instanceId << std::endl;
        return;
    }

    /*WorkElement *element = instance->findElement(elementName);*/
    WorkElement* element = nullptr;
    for (DepthFirstIterator it(instance->getRoot()); !it.isDone(); it.next()){
        if(it.current()->getName() == elementName) {
            element = it.current();
        }
    }
    
    if (element == nullptr)
    {
        std::cout << "No work element found with name: " << elementName << " in instance ID: " << instanceId << std::endl;
        return;
    }
    element->complete();

    if (instance->getRoot()->isComplete())
    {
        if (instance->getStateName() == "Running") instance->complete();
    }
    else
    {
        activateNext(*instance);
    }
}

void WorkflowEngine::handleIssue(const Issue &issue, WorkflowInstance &instance)
{
    //instance.publish(WorkflowEvent("IssueRaised", instance.getId(), issue.getElementName(), issue.getDescription()));
    instance.publish("IssueRaised");

    EscalationHandler *chain = instance.getDefinition()->getEscalationHandler();
    if (chain == nullptr || !chain->handleIssue(issue))
    {
        std::cout << "Issue could not be resolved: " << issue.getDescription() << std::endl;
        instance.fail(issue);
        return;
    }

    if (!issue.getElementName().empty())
    {
        /*WorkElement *element = instance.findElement(issue.getElementName());*/
        WorkElement* element = nullptr;
        for (DepthFirstIterator it(instance.getRoot()); !it.isDone(); it.next()) {
            if(it.current()->getName() == issue.getElementName()){
                element = it.current();
                break;
            }
        }
        
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
            element->setStatus(WorkItemStatus::CREATED);
        }
    }

    //instance.publish(WorkflowEvent("IssueResolved", instance.getId(), issue.getElementName(), issue.getDescription()));
    instance.publish("IssueResolved");
    activateNext(instance);
}

void WorkflowEngine::assignWork(WorkflowInstance &instance, WorkElement *element)
{
    if (element == nullptr)
    {
        return;
    }

    AssignmentStrategy *rule = instance.getDefinition()->getAssignmentStrategy();
    AssignmentStrategy *previous = assignment->getStrategy();
    if (rule != nullptr)
    {
        assignment->setStrategy(rule);
    }

    Participant *participant = assignment->assignWorkItem(*element, instance.getData(), participants);
    assignment->setStrategy(previous);
    
    if (participant == nullptr)
    {
        Issue issue("NoCandidate", "No suitable participant for " + element->getName(), 2, element->getName());
        handleIssue(issue, instance);
        return;
    }

    element->assignTo(participant);
    element->setStatus(WorkItemStatus::ASSIGNED);
    participant->incrementWorkload();
    instance.publish("ElementAssigned");
}

void WorkflowEngine::activateNext(WorkflowInstance &instance)
{
    if (instance.getStateName() != "Running")
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
        element->setStatus(WorkItemStatus::AVAILABLE);

        if (element->requiresAssignee())
        {
            assignWork(instance, element);

            if (instance.getStateName() != "Running")
            {
                return;
            }
        }
    }
    instance.getRoot()->execute();
}

WorkflowInstance *WorkflowEngine::getInstance(int id) const
{
    std::map<int, WorkflowInstance *>::const_iterator it = instances.find(id);
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
Participant *WorkflowEngine::findParticipant(int id) const
{
    for (std::vector<Participant *>::const_iterator it = participants.begin(); it != participants.end(); ++it)
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
