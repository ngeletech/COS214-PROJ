#include "../include/Adapter.h"
#include <string>
using namespace std;

ServiceResponse::ServiceResponse(string reference, string status, bool ok) {
    this->reference = reference;
    this->status = status;
    this->ok = ok;
}

string ServiceResponse::getReference() {
    return reference;
}

string ServiceResponse::getStatus() {
    return status;
}

bool ServiceResponse::isOk() {
    return ok;
}