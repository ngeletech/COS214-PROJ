#include "../include/Decorator.h"
#include <string>
using namespace std;

PriorityDecorator::PriorityDecorator(WorkElement* element, int priority) : WorkElementDecorator(element) {
    this->priority = priority;
}

void PriorityDecorator::execute() {
    if(!wrapped) return;
    wrapped->execute();
}

int PriorityDecorator::getPriority() {
    return priority;
}

void PriorityDecorator::setPriority(int priority) {
    this->priority = priority;
}