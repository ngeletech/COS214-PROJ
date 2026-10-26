#include "../include/Adapter.h"
#include "../include/WorkData.h"
#include <string>
using namespace std;

RestApiAdapter::RestApiAdapter(RestApiClient* client) : client(client) {}

RestApiAdapter::~RestApiAdapter() {}

ServiceResponse RestApiAdapter::sendRequest(const ServiceRequest& req) {
    string body = "";
    const map<string, string> data = req.getPayload().getData();
    for (auto const& pair : data) {
        if (!body.empty()) body += "&";
        body += pair.first + "=" + pair.second;
    }

    string refId = client->post("/requests", body);
    bool success = !refId.empty();

    return ServiceResponse(refId, "accepted", success);
}

string RestApiAdapter::queryStatus(string reference) {
    return client->get("/requests/" + reference);
}