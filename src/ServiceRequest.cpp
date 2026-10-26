#include "../include/Adapter.h"
#include "../include/WorkData.h"

ServiceRequest::ServiceRequest(WorkData payload) {
    this->payload = payload;
}

const WorkData& ServiceRequest::getPayload() const{
    return payload;
}