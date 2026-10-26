#include "../include/Adapter.h"
#include "../include/WorkData.h"
#include <string>
using namespace std;

int MessagingAdapter::counter = 0;

ServiceResponse MessagingAdapter::sendRequest(const ServiceRequest& req) {
    string body = "";
    const map<string, string> data = req.getPayload().getData();
    for (auto const& [key, val] : data) {
        if (!body.empty()) body += "&";
        body += key + "=" + val;
    }
 
    string id = "MSG-" + to_string(++counter);    
    queue.publish("requests", id + "|" + body);
 
    return ServiceResponse(id, "queued", true);
}
 
string MessagingAdapter::queryStatus(string reference) {
    return queue.poll(reference);
}
