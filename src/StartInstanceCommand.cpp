#include "WorkflowCommand.h"
#include "WorkflowEngine.h"

StartInstanceCommand::StartInstanceCommand(WorkflowEngine* engine, std::string defName, WorkData data) : WorkflowCommand(nullptr), engine(engine), defName(defName), data(data) {}
void StartInstanceCommand::execute() {
    WorkflowInstance* started = engine->startInstance(defName, data);
    if(started != nullptr) {
        engine->activateNext(*started);
    }
}
