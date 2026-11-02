#include "../include/Adapter.h"
#include <string>
using namespace std;

int RestApiClient::counter = 1000;

string RestApiClient::post(string url, string body) {
    int id = ++counter;
    string ref = "REF-" + std::to_string(id);
    statuses[ref] = isFailingId(id) ? "failed" : "pending";
    return ref;
}

string RestApiClient::get(string url) {
    string ref = url.substr(url.find_last_of('/') + 1);

    auto it = statuses.find(ref);
    if(it == statuses.end()) {
        return "unknown";
    }

    string& status = it->second;
    if(status == "pending") status = "processing";
    else if(status == "processing") status = "delivered";

    return status;
}