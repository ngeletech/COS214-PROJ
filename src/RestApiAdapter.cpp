#include "../include/Adapter.h"
#include "../include/WorkData.h"
#include <string>
using namespace std;

RestApiAdapter::RestApiAdapter(RestApiClient* client) {
    this->client = client ? client : new RestApiClient();
}

RestApiAdapter::~RestApiAdapter() {
    delete client;
}

ServiceResponse RestApiAdapter::sendRequest(const ServiceRequest& req) {
    string body = "";
    const map<string, string> data = req.getPayload().getData();
    for (auto const& [key, val] : data) {
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