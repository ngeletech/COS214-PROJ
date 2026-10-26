#include "../include/Adapter.h"
#include "../include/WorkData.h"
#include <string>
using namespace std;

RestApiAdapter::RestApiAdapter(RestApiClient* client) {
    this->client = client;
}

RestApiAdapter::~RestApiAdapter() {}

ServiceResponse RestApiAdapter::sendRequest(const ServiceRequest& req) {
    string body = "";
    for (auto const& [key, val] : req.getPayload()) {
        if (!body.empty()) body += "&";
        body += key + "=" + val;
    }

    string refId = client->post("/requests", body);
    bool success = !refId.empty();

    return ServiceResponse(refId, "accepted", success);
}

string RestApiAdapter::queryStatus(string reference) {
    return client->get("/requests/" + reference);
}