#include "TividyFacade.h"
#include "State.h"
#include "composite.h"
#include "Iterator.h"
#include "Participant.h"
#include "Observer.h"
#include "WorkflowEngine.h"
#include "WorkflowCommand.h"
#include "AssignmentStrategy.h"
#include "WorkItemFactory.h"
#include "InstanceSnapshot.h"
#include "TemplateMethod.h"
#include "Issue.h"

#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <map>
#include <memory>

//Helper: Turns status enum into text
static std::string statusText(WorkItemStatus status) {
    switch (status) {
        case WorkItemStatus::CREATED:   return "created";
        case WorkItemStatus::AVAILABLE: return "available";
        case WorkItemStatus::ASSIGNED:  return "assigned";
        case WorkItemStatus::STARTED:   return "started";
        case WorkItemStatus::COMPLETED: return "completed";
        case WorkItemStatus::REJECTED:  return "rejected";
        case WorkItemStatus::CANCELLED: return "cancelled";
    }
    return "unknown";
}

TividyFacade::TividyFacade() {
    audit = new AuditHistory();
    rollback = new RollbackManager();
    invoker = new CommandInvoker(rollback);
    defaultStrategy = new LeastLoadedStrategy();
    assignment = new AssignmentService(defaultStrategy);
    factories = new WorkItemFactoryRegistry();   // registers the 6 standard factories itself
    engine = new WorkflowEngine(assignment, factories);
    designer = new WorkflowDesigner(new StandardDefinitionBuilder());
    auditLogger = new AuditLogger(audit);
    activator = new NextItemActivator(engine);

    engine->addDefaultListener(auditLogger);
    engine->addDefaultListener(activator);
}

TividyFacade::~TividyFacade() {
    delete invoker;        // history points into instances the engine owns
    delete engine;         // deletes instances and definitions
    delete designer;       // check whether it deletes its builder
    delete assignment;
    delete defaultStrategy;
    delete factories;
    delete auditLogger;
    delete activator;
    for (size_t k = 0; k < listeners.size(); ++k) {
        delete listeners[k];
    } 
    for (size_t k = 0; k < participants.size(); ++k) {
        delete participants[k];
    }
    delete rollback;
    delete audit;
}

WorkflowInstance& TividyFacade::requireInstance(int id) const {
    WorkflowInstance* inst = engine->getInstance(id);
    if (inst == nullptr) throw std::invalid_argument("Unknown instance id");
    return *inst;
}

void TividyFacade::run(WorkflowCommand* command) {
    invoker->submit(command);
    invoker->runNext();
}

WorkflowDefinition* TividyFacade::defineWorkflow(const std::string& name,
        const std::vector<WorkItemSpec>& specs,
        AssignmentStrategy* strategy, EscalationHandler* chain) {
    WorkflowDefinition* def = designer->construct(name, specs, strategy, chain);
    engine->registerDefinition(def);
    if (std::find(definitionNames.begin(), definitionNames.end(), name)
            == definitionNames.end())
        definitionNames.push_back(name);
    return def;
}

std::vector<std::string> TividyFacade::listDefinitions() const {
    return definitionNames;
}

int TividyFacade::startWorkflow(const std::string& defName, const WorkData& data) {
    WorkflowInstance* inst = engine->startInstance(defName, data);      // creates and starts
    if (inst == nullptr) throw std::invalid_argument("Unknown workflow: " + defName);               
    engine->activateNext(*inst);    // assign the first available items
    return inst->getId();
}

void TividyFacade::cancelWorkflow(int id) {
    run(new CancelInstanceCommand(&requireInstance(id)));
}

void TividyFacade::changeData(int id, const std::string& key, const std::string& value) {
    run(new ChangeDataCommand(&requireInstance(id), key, value));
}

void TividyFacade::reassignItem(int id, const std::string& elementName, Participant* p) {
    if (p == nullptr) {
        throw std::invalid_argument("No participant given");
    }
    run(new ReassignItemCommand(&requireInstance(id), elementName, p));
}

void TividyFacade::completeItem(int id, const std::string& elementName) {
    WorkflowInstance& inst = requireInstance(id);

    WorkElement* found = nullptr;
    if (inst.getRoot() != nullptr) {
        for (DepthFirstIterator it(inst.getRoot()); !it.isDone(); it.next()) {
            if (it.current()->getName() == elementName) { found = it.current(); break; }
        }
    }
    if (found == nullptr) {
        throw std::invalid_argument("Unknown work item: " + elementName);
    }

    if (inst.getStateName() != "Running") {
        throw std::logic_error("Cannot complete an item while " + inst.getStateName());
    }
    
    engine->completeWorkItem(id, elementName);
}

void TividyFacade::decideItem(int id, const std::string& elementName, bool approved) {
    WorkflowInstance& inst = requireInstance(id);
    if (inst.getStateName() != "Running")
        throw std::logic_error("Cannot decide an item while " + inst.getStateName());
    ApprovalTask* task = dynamic_cast<ApprovalTask*>(inst.findElement(elementName));
    if (task == nullptr)
        throw std::invalid_argument("Not an approval item: " + elementName);
    task->recordDecision(approved);
    if (approved) {
        engine->completeWorkItem(id, elementName);
    } else {
        Issue issue("Rejected", elementName + " was rejected", 3, elementName);
        engine->handleIssue(issue, inst);
    }
}

void TividyFacade::reportIssue(int id, const std::string& elementName,
        const std::string& type, const std::string& description, int severity) {
    WorkflowInstance& inst = requireInstance(id);
    if (inst.getStateName() != "Running")
        throw std::logic_error("Cannot report an issue while " + inst.getStateName());
    Issue issue(type, description, severity, elementName);
    engine->handleIssue(issue, inst);
}

void TividyFacade::undoLast() {
    invoker->undoLast();
}

std::string TividyFacade::trackStatus(int id) const {
    return requireInstance(id).getStateName();
}

std::vector<std::string> TividyFacade::getAvailableWork(int id) const {
    std::vector<std::string> names;
    WorkflowInstance& inst = requireInstance(id);

    if (inst.getRoot() == nullptr) {
        return names;
    }

    for (DepthFirstIterator it(inst.getRoot()); !it.isDone(); it.next()) {
        WorkElement* e = it.current();
        if (dynamic_cast<WorkGroup*>(e) != nullptr) continue;   // skip groups
        WorkItemStatus s = e->getStatus();
        if (s == WorkItemStatus::AVAILABLE || s == WorkItemStatus::ASSIGNED ||s == WorkItemStatus::STARTED) {
            names.push_back(e->getName());
        }    
    }

    return names;
}

std::string TividyFacade::generateReport(int id) const {
     WorkflowInstance& inst = requireInstance(id);
    std::ostringstream out;

    out << "=== Workflow instance " << id << " ===\n";
    out << "State: " << inst.getStateName() << "\n";

    // data
    out << "\nData:\n";
    const std::map<std::string, std::string>& values = inst.getData().getData();
    if (values.empty()) {
        out << "  (none)\n";
    }
    for (std::map<std::string, std::string>::const_iterator it = values.begin();
         it != values.end(); ++it) {
        out << "  " << it->first << " = " << it->second << "\n";
    }

    // progress: every task in the work tree
    out << "\nWork items:\n";
    for (DepthFirstIterator it(inst.getRoot()); !it.isDone(); it.next()) {
        WorkElement* element = it.current();
        if (element == inst.getRoot()) continue;               // skip the hidden root group
        out << "  " << element->getName() << ": " << statusText(element->getStatus());
        Participant* who = element->getAssignee();
        if (who != nullptr) out << " (" << who->getName() << ")";
        out << "\n";
    }

    // history: the audit entries for this instance only
    out << "\nHistory:\n";
    std::unique_ptr<WorkIterator<const AuditEntry> > entries(audit->createIterator());
    int count = 0;
    for (entries->first(); !entries->isDone(); entries->next()) {
        const AuditEntry* entry = entries->current();
        if (entry->getInstanceId() != id) continue;
        out << "  " << entry->getEventType();
        if (!entry->getDetails().empty()) out << " - " << entry->getDetails();
        out << "\n";
        ++count;
    }
    if (count == 0) out << "  (no events recorded)\n";

    return out.str();
}

void TividyFacade::registerParticipant(Participant* participant) {
    if (participant == nullptr) {
        return;
    }
    participants.push_back(participant);
    engine->addParticipant(participant);
}

void TividyFacade::addListener(WorkflowListener* listener) {
    if (listener == nullptr) {
        return;
    }
    listeners.push_back(listener);
    engine->addDefaultListener(listener);
}