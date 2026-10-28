#include "../include/Decorator.h"
#include <string>
#include <iostream>
using namespace std;

AuditedDecorator::AuditedDecorator(WorkElement* element) : WorkElementDecorator(element) {}

void AuditedDecorator::execute() {
    if(!wrapped) return;
    cout << "[AUDIT] before execute" << endl;
    wrapped->execute();
    cout << "[AUDIT] after execute" << endl;
}

void AuditedDecorator::complete() {
    if(!wrapped) return;
    wrapped->complete();
}