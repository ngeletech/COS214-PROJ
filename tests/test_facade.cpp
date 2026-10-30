#include "doctest.h"
#include "TividyFacade.h"
#include "Participant.h"
#include "Chain-Of-Responsibility.h"
#include "AssignmentStrategy.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

// Type strings must match the keys in WorkItemFactoryRegistry.
static std::vector<WorkItemSpec> specsFor(const std::string& type, const std::string& name = "item1") {
    std::vector<WorkItemSpec> specs;
    WorkItemSpec s = { type, name, "" };
    specs.push_back(s);
    return specs;
}

// The facade owns participants and the chain, so they are heap-allocated.
static int startFlow(TividyFacade& f, const std::string& flow, const std::string& type,
                     EscalationHandler* chain = nullptr) {
    f.registerParticipant(new Participant(1, "Thandi", "tester"));
    f.defineWorkflow(flow, specsFor(type), nullptr, chain);
    return f.startWorkflow(flow, WorkData());
}

static bool isListed(const std::vector<std::string>& names, const std::string& name) {
    return std::find(names.begin(), names.end(), name) != names.end();
}

TEST_CASE("A defined workflow is listed") {
    TividyFacade facade;
    facade.defineWorkflow("Simple", specsFor("UserTask"), nullptr, nullptr);
    std::vector<std::string> names = facade.listDefinitions();
    REQUIRE(names.size() == 1);
    CHECK(names[0] == "Simple");
}

TEST_CASE("Starting a workflow runs it and offers its work") {
    TividyFacade facade;
    int id = startFlow(facade, "Simple", "UserTask");
    CHECK(facade.trackStatus(id) == "Running");
    std::vector<std::string> work = facade.getAvailableWork(id);
    REQUIRE(work.size() == 1);
    CHECK(work[0] == "item1");
}

TEST_CASE("Completing the only item completes the workflow") {
    TividyFacade facade;
    int id = startFlow(facade, "Simple", "UserTask");
    facade.completeItem(id, "item1");
    CHECK(facade.trackStatus(id) == "Completed");
    CHECK(facade.getAvailableWork(id).empty());
}

TEST_CASE("Unknown workflows, ids and items are refused") {
    TividyFacade facade;
    CHECK_THROWS_AS(facade.startWorkflow("Nope", WorkData()), std::invalid_argument);
    CHECK_THROWS_AS(facade.trackStatus(99), std::invalid_argument);
    CHECK_THROWS_AS(facade.cancelWorkflow(99), std::invalid_argument);
    CHECK_THROWS_AS(facade.decideItem(99, "item1", true), std::invalid_argument);

    int id = startFlow(facade, "Simple", "UserTask");
    CHECK_THROWS_AS(facade.completeItem(id, "missing"), std::invalid_argument);
    CHECK_THROWS_AS(facade.decideItem(id, "item1", true), std::invalid_argument);  // not an approval
    CHECK(facade.trackStatus(id) == "Running");
}

TEST_CASE("A cancelled workflow refuses every further action") {
    TividyFacade facade;
    int id = startFlow(facade, "Simple", "UserTask");
    facade.cancelWorkflow(id);
    CHECK(facade.trackStatus(id) == "Cancelled");

    CHECK_THROWS_AS(facade.cancelWorkflow(id), std::logic_error);
    CHECK_THROWS_AS(facade.completeItem(id, "item1"), std::logic_error);
    CHECK_THROWS_AS(facade.decideItem(id, "item1", true), std::logic_error);
    CHECK_THROWS_AS(facade.reportIssue(id, "item1", "X", "Y", 1), std::logic_error);
}

TEST_CASE("Workflows from one definition are isolated, including state, data and history") {
    TividyFacade facade;
    facade.registerParticipant(new Participant(1, "Thandi", "tester"));
    facade.defineWorkflow("Simple", specsFor("UserTask"), nullptr, nullptr);
    int a = facade.startWorkflow("Simple", WorkData());
    int b = facade.startWorkflow("Simple", WorkData());

    facade.cancelWorkflow(a);
    facade.changeData(b, "coupon", "SAVE10");

    CHECK(facade.trackStatus(a) == "Cancelled");
    CHECK(facade.trackStatus(b) == "Running");
    CHECK(facade.generateReport(b).find("SAVE10") != std::string::npos);
    CHECK(facade.generateReport(a).find("SAVE10") == std::string::npos);
    CHECK(facade.generateReport(a).find("InstanceCancelled") != std::string::npos);
    CHECK(facade.generateReport(b).find("InstanceCancelled") == std::string::npos);
}

TEST_CASE("Undo reverses the last change") {
    TividyFacade facade;
    int id = startFlow(facade, "Simple", "UserTask");
    facade.cancelWorkflow(id);
    REQUIRE(facade.trackStatus(id) == "Cancelled");
    facade.undoLast();
    CHECK(facade.trackStatus(id) == "Running");
}

TEST_CASE("Approving an approval item completes the workflow") {
    TividyFacade facade;
    int id = startFlow(facade, "Approve", "ApprovalTask");
    CHECK(isListed(facade.getAvailableWork(id), "item1"));
    facade.decideItem(id, "item1", true);
    CHECK(facade.trackStatus(id) == "Completed");
}

TEST_CASE("Rejecting sends the item back for rework when the chain resolves it") {
    TividyFacade facade;
    int id = startFlow(facade, "Reject", "ApprovalTask", new FrontlineHandler(nullptr, 5));
    facade.decideItem(id, "item1", false);
    CHECK(facade.trackStatus(id) == "Running");
    CHECK(isListed(facade.getAvailableWork(id), "item1"));
}

TEST_CASE("Rejecting with no chain fails the instance") {
    TividyFacade facade;
    int id = startFlow(facade, "RejectFails", "ApprovalTask");
    facade.decideItem(id, "item1", false);
    CHECK(facade.trackStatus(id) == "Failed");
}

TEST_CASE("An issue fails the instance unless the chain resolves it") {
    TividyFacade unresolved;
    int a = startFlow(unresolved, "Unresolved", "UserTask");
    unresolved.reportIssue(a, "item1", "Damaged", "Box crushed", 4);
    CHECK(unresolved.trackStatus(a) == "Failed");

    TividyFacade resolved;
    int b = startFlow(resolved, "Resolved", "UserTask", new FrontlineHandler(nullptr, 5));
    resolved.reportIssue(b, "item1", "Delay", "Late pickup", 2);
    CHECK(resolved.trackStatus(b) == "Running");
}

TEST_CASE("The definition's assignment rule picks the participant") {
    TividyFacade facade;
    Participant* a = new Participant(1, "Ann", "nurse");
    Participant* b = new Participant(2, "Ben", "doctor");
    facade.registerParticipant(a);
    facade.registerParticipant(b);
    facade.defineWorkflow("Rule", specsFor("UserTask"), new RoleMatchingStrategy("doctor"), nullptr);
    int id = facade.startWorkflow("Rule", WorkData());

    CHECK(b->getWorkload() == 1);
    CHECK(a->getWorkload() == 0);
    CHECK(facade.trackStatus(id) == "Running");
}

TEST_CASE("An unfillable rule with a resolving chain does not loop forever") {
    TividyFacade facade;
    facade.registerParticipant(new Participant(1, "Ann", "nurse"));
    facade.defineWorkflow("NoDoctor", specsFor("UserTask"),
                          new RoleMatchingStrategy("doctor"), new FrontlineHandler(nullptr, 5));
    CHECK_NOTHROW(facade.startWorkflow("NoDoctor", WorkData()));
}