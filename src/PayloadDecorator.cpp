#include "../include/Decorator.h"
#include "../include/WorkData.h"
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

WorkData PayloadDecorator::getPayload() const {
    WorkData data = wrapped->getPayload();
    data.addData(key, value);
    return data;
}