#include "WorkflowCommand.h"

ReassignItemCommand::ReassignItemCommand(WorkflowInstance* target, std::string elementName, Participant* newAssignee) : WorkflowCommand(target), elementName(elementName), newAssignee(newAssignee) {}
void ReassignItemCommand::execute() {
    WorkElement* element = target->findElement(elementName);
    Participant* currentAssignee = element->getAssignee();
    if (currentAssignee != nullptr) currentAssignee->decrementWorkload();
    element->assignTo(newAssignee);
    newAssignee->incrementWorkload();
}