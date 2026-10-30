#include "../include/Decorator.h"
#include <string>
#include <iostream>
using namespace std;

AuditedDecorator::AuditedDecorator(WorkElement* element) : WorkElementDecorator(element) {}

void AuditedDecorator::execute() {
    if(!wrapped) return;
    cout << "[AUDIT] before execution" << endl;
    wrapped->execute();
    cout << "[AUDIT] after execution" << endl;
}

void AuditedDecorator::complete() {
    if(!wrapped) return;
    cout << "[AUDIT] before completion" << endl;
    wrapped->complete();
    cout << "[AUDIT] after completion" << endl;
}