#include "../include/Adapter.h"
#include "../include/WorkData.h"

ServiceRequest::ServiceRequest(WorkData payload) :payload(payload) {}

const WorkData& ServiceRequest::getPayload() const{
    return payload;
}