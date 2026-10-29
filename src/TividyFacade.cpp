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
#include "ReportGenerator.h"
#include "InstanceSnapshot.h"
#include <stdexcept>
#include <algorithm>

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
    reports = new ReportGenerator(audit);

    engine->addDefaultListener(auditLogger);
    engine->addDefaultListener(activator);
}

TividyFacade::~TividyFacade() {
    delete invoker;        // history points into instances the engine owns
    delete engine;         // deletes instances and definitions
    delete designer;       // check whether it deletes its builder
    delete reports;
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
    WorkflowInstance* inst = engine->startInstance(defName, data);
    if (inst == nullptr) throw std::invalid_argument("Unknown workflow: " + defName);
    inst->start();                 // Created -> Running
    engine->activateNext(*inst);
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
    requireInstance(id);
    engine->completeWorkItem(id, elementName);
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

    for (AvailableItemIterator it(inst.getRoot()); !it.isDone(); it.next()) {
        names.push_back(it.current()->getName());
    }
    return names;
}

std::string TividyFacade::generateReport(int id) const {
    return reports->progressReport(requireInstance(id));
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