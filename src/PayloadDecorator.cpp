#include "../include/Decorator.h"
#include <string>
using namespace std;

PayloadDecorator::PayloadDecorator(WorkElement* element, string key, string value) : WorkElementDecorator(element) {
    this->key = key;
    this->value = value;
}

void PayloadDecorator::execute() {
    if(!wrapped) return;
    wrapped->execute();
}

WorkData PayloadDecorator::getPayload() {
    if(!wrapped) return;
    return wrapped->getPayload();
}