#include "../include/Adapter.h"
#include "../include/WorkData.h"
#include <string>
using namespace std;

LegacySystemAdapter::LegacySystemAdapter(LegacySystemApi* api) {
    this->api = api ? api : new LegacySystemApi();
}

LegacySystemAdapter::~LegacySystemAdapter() {
    delete api;
}

ServiceResponse LegacySystemAdapter::sendRequest(const ServiceRequest& req) {
    string record = "";
    for (auto const& pair : req.getPayload().getData()) {
        string k = pair.first; k.resize(15, ' ');
        string v = pair.second; v.resize(15, ' ');
        record += k + v;
    }

    string code = api->submitRecord(record);

    bool ok = (code != "ERR");
    return ServiceResponse(code, ok ? "submitted" : "rejected", ok);
}

string LegacySystemAdapter::queryStatus(string reference) {
    string code = api->lookup(reference);
    if (code == "00") return "pending";
    if (code == "01") return "processing";
    if (code == "02") return "delivered";
    if (code == "99") return "failed";
    return "unknown";
}