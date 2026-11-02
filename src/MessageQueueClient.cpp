#include "../include/Adapter.h"
#include <string>
using namespace std;

void MessageQueueClient::publish(string subject, string message) {
    size_t bar = message.find('|');
    string id = (bar == string::npos) ? message : message.substr(0, bar);
    messages[id] = "queued";
}

string MessageQueueClient::poll(string correlationId) {
    auto it = messages.find(correlationId);
    if(it == messages.end()) return "unknown";

    string& status = it->second;
    string result = status;

    if(status == "queued") status ="processing";
    else if (status == "processing") status = "delivered";
    return result;
}