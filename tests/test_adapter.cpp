#include "doctest.h"
#include "Adapter.h"
#include "WorkData.h"

// Helper function to keep tests clean
WorkData makeData() {
    WorkData data;
    data.addData("name", "Alice");
    data.addData("role", "Approver");
    return data;
}

// ---------- ServiceRequest / ServiceResponse ----------
TEST_CASE("ServiceRequest stores payload") {
    WorkData data = makeData();
    ServiceRequest req(data);
    
    CHECK(req.getPayload().getData().at("name") == "Alice");
    CHECK(req.getPayload().getData().at("role") == "Approver");
}

// ---------- RestApiAdapter ----------
TEST_CASE("RestApiAdapter sendRequest returns ok") {
    RestApiClient client;
    RestApiAdapter adapter(&client);

    WorkData data = makeData();
    ServiceResponse resp = adapter.sendRequest(ServiceRequest(data));

    CHECK(resp.isOk());
    CHECK(resp.getReference().substr(0, 4) == "REF-");
    CHECK(resp.getStatus() == "accepted");
}

TEST_CASE("RestApiAdapter queryStatus progresses") {
    RestApiClient client;
    RestApiAdapter adapter(&client);
    
    WorkData data = makeData();
    auto resp = adapter.sendRequest(ServiceRequest(data));

    CHECK(adapter.queryStatus(resp.getReference()) == "processing");
    CHECK(adapter.queryStatus(resp.getReference()) == "delivered");
    CHECK(adapter.queryStatus(resp.getReference()) == "delivered"); // terminal
}

// ---------- LegacySystemAdapter ----------
TEST_CASE("LegacySystemAdapter sendRequest ok") {
    LegacySystemApi api;
    LegacySystemAdapter adapter(&api);

    WorkData data = makeData();
    auto resp = adapter.sendRequest(ServiceRequest(data));

    CHECK(resp.isOk());
    CHECK(resp.getReference() == "OK0001");
}

TEST_CASE("LegacySystemAdapter queryStatus translation") {
    LegacySystemApi api;
    LegacySystemAdapter adapter(&api);
    
    WorkData data = makeData();
    auto resp = adapter.sendRequest(ServiceRequest(data));
    std::string ref = resp.getReference();

    CHECK(adapter.queryStatus(ref) == "pending");
    CHECK(adapter.queryStatus(ref) == "processing");
    CHECK(adapter.queryStatus(ref) == "delivered");
}

// ---------- MessagingAdapter ----------
TEST_CASE("MessagingAdapter sendRequest queues") {
    MessagingAdapter adapter;
    
    WorkData data = makeData();
    auto resp = adapter.sendRequest(ServiceRequest(data));

    CHECK(resp.isOk());
    CHECK(resp.getReference().substr(0, 4) == "MSG-");
    CHECK(resp.getStatus() == "queued");
}