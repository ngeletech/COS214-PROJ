#include "../include/Adapter.h"
#include <string>
using namespace std;

ServiceResponse::ServiceResponse(string reference, string status, bool ok) : reference(reference), status(status), ok(ok) {}


string ServiceResponse::getReference() const {
    return reference;
}

string ServiceResponse::getStatus() const {
    return status;
}

bool ServiceResponse::isOk() const {
    return ok;
}