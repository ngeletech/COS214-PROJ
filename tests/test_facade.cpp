#include "doctest.h"
#include "TividyFacade.h"
#include "Participant.h"
#include "Chain-Of-Responsibility.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

// The type string must match a key in WorkItemFactoryRegistry.
// Check it with: grep -n "registerFactory" src/WorkItemFactoryRegistry.cpp
static const char* USER_TYPE = "UserTask";

static std::vector<WorkItemSpec> specsFor(const std::string& name) {
    std::vector<WorkItemSpec> specs;
    WorkItemSpec s = { USER_TYPE, name, "" };
    specs.push_back(s);
    return specs;
}

// The facade owns participants, so they must be heap-allocated.
static void addWorker(TividyFacade& facade) {
    facade.registerParticipant(new Participant(1, "Thandi", "tester"));
}

// One worker, one workflow with a single item "item1" of the given type.
static int startSingleItemFlow(TividyFacade& facade, const std::string& flowName,
                               const std::string& type, EscalationHandler* chain) {
    addWorker(facade);
    std::vector<WorkItemSpec> specs;
    WorkItemSpec s = { type, "item1", "" };
    specs.push_back(s);
    facade.defineWorkflow(flowName, specs, nullptr, chain);
    return facade.startWorkflow(flowName, WorkData());
}

static bool isListed(const std::vector<std::string>& names, const std::string& name) {
    return std::find(names.begin(), names.end(), name) != names.end();
}

TEST_CASE("A defined workflow is listed") {
    TividyFacade facade;
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    std::vector<std::string> names = facade.listDefinitions();
    REQUIRE(names.size() == 1);
    CHECK(names[0] == "Simple");
}

TEST_CASE("Unknown workflows and ids are refused") {
    TividyFacade facade;
    CHECK_THROWS_AS(facade.startWorkflow("Nope", WorkData()), std::invalid_argument);
    CHECK_THROWS_AS(facade.trackStatus(99), std::invalid_argument);
    CHECK_THROWS_AS(facade.cancelWorkflow(99), std::invalid_argument);
    CHECK_THROWS_AS(facade.completeItem(99, "step1"), std::invalid_argument);
}

TEST_CASE("Starting a workflow runs it and offers work") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);

    int id = facade.startWorkflow("Simple", WorkData());
    CHECK(facade.trackStatus(id) == "Running");
    std::vector<std::string> work = facade.getAvailableWork(id);
    REQUIRE(work.size() == 1);
    CHECK(work[0] == "step1");
}

TEST_CASE("Completing the only item completes the workflow") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    int id = facade.startWorkflow("Simple", WorkData());

    facade.completeItem(id, "step1");
    CHECK(facade.trackStatus(id) == "Completed");
    CHECK(facade.getAvailableWork(id).empty());
}

TEST_CASE("An unknown work item is refused") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    int id = facade.startWorkflow("Simple", WorkData());

    CHECK_THROWS_AS(facade.completeItem(id, "missing"), std::invalid_argument);
    CHECK(facade.trackStatus(id) == "Running");
}

TEST_CASE("A cancelled workflow cannot be cancelled or completed again") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    int id = facade.startWorkflow("Simple", WorkData());

    facade.cancelWorkflow(id);
    CHECK(facade.trackStatus(id) == "Cancelled");
    CHECK_THROWS_AS(facade.cancelWorkflow(id), std::logic_error);
    CHECK_THROWS_AS(facade.completeItem(id, "step1"), std::logic_error);
}

TEST_CASE("Two workflows from one definition are independent") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    int a = facade.startWorkflow("Simple", WorkData());
    int b = facade.startWorkflow("Simple", WorkData());

    facade.cancelWorkflow(a);
    facade.changeData(b, "coupon", "SAVE10");

    CHECK(facade.trackStatus(a) == "Cancelled");
    CHECK(facade.trackStatus(b) == "Running");
    CHECK(facade.generateReport(b).find("SAVE10") != std::string::npos);
    CHECK(facade.generateReport(a).find("SAVE10") == std::string::npos);
}

TEST_CASE("Undo reverses the last change") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    int id = facade.startWorkflow("Simple", WorkData());

    facade.cancelWorkflow(id);
    REQUIRE(facade.trackStatus(id) == "Cancelled");
    facade.undoLast();
    CHECK(facade.trackStatus(id) == "Running");
}

TEST_CASE("The report shows the state, the work item and only its own history") {
    TividyFacade facade;
    addWorker(facade);
    facade.defineWorkflow("Simple", specsFor("step1"), nullptr, nullptr);
    int a = facade.startWorkflow("Simple", WorkData());
    int b = facade.startWorkflow("Simple", WorkData());
    facade.cancelWorkflow(a);

    std::string reportA = facade.generateReport(a);
    CHECK(reportA.find("Cancelled") != std::string::npos);
    CHECK(reportA.find("step1") != std::string::npos);
    CHECK(reportA.find("InstanceCancelled") != std::string::npos);
    CHECK(facade.generateReport(b).find("InstanceCancelled") == std::string::npos);
}

// ---------------- approvals and issues ----------------

TEST_CASE("Approving an approval item completes the workflow") {
    TividyFacade facade;
    int id = startSingleItemFlow(facade, "ApproveFlow", "ApprovalTask", nullptr);
    CHECK(isListed(facade.getAvailableWork(id), "item1"));

    facade.decideItem(id, "item1", true);

    CHECK(facade.trackStatus(id) == "Completed");
    CHECK_FALSE(isListed(facade.getAvailableWork(id), "item1"));
}

TEST_CASE("Rejecting with a resolving chain sends the item back for rework") {
    TividyFacade facade;
    // Rejection raises severity 3, which this handler can resolve.
    int id = startSingleItemFlow(facade, "RejectResolved", "ApprovalTask",
                                 new FrontlineHandler(nullptr, 5));

    facade.decideItem(id, "item1", false);

    CHECK(facade.trackStatus(id) == "Running");
    CHECK(isListed(facade.getAvailableWork(id), "item1"));
}

TEST_CASE("Rejecting with no chain fails the instance") {
    TividyFacade facade;
    int id = startSingleItemFlow(facade, "RejectFails", "ApprovalTask", nullptr);

    facade.decideItem(id, "item1", false);

    CHECK(facade.trackStatus(id) == "Failed");
}

TEST_CASE("decideItem refuses non-approval items, unknown items and cancelled instances") {
    TividyFacade facade;
    int id = startSingleItemFlow(facade, "NotApproval", "UserTask", nullptr);

    CHECK_THROWS_AS(facade.decideItem(id, "item1", true), std::invalid_argument);
    CHECK_THROWS_AS(facade.decideItem(id, "missing", true), std::invalid_argument);
    CHECK_THROWS_AS(facade.decideItem(999, "item1", true), std::invalid_argument);

    facade.cancelWorkflow(id);
    CHECK_THROWS_AS(facade.decideItem(id, "item1", true), std::logic_error);
}

TEST_CASE("reportIssue fails the instance when nobody can resolve it") {
    TividyFacade facade;
    int id = startSingleItemFlow(facade, "IssueFails", "UserTask", nullptr);

    facade.reportIssue(id, "item1", "Damaged", "Box crushed", 4);

    CHECK(facade.trackStatus(id) == "Failed");
}

TEST_CASE("reportIssue resolved by the chain keeps the instance running") {
    TividyFacade facade;
    int id = startSingleItemFlow(facade, "IssueResolved", "UserTask",
                                 new FrontlineHandler(nullptr, 5));

    facade.reportIssue(id, "item1", "Delay", "Late pickup", 2);

    CHECK(facade.trackStatus(id) == "Running");
}

TEST_CASE("reportIssue refuses unknown and cancelled instances") {
    TividyFacade facade;
    int id = startSingleItemFlow(facade, "IssueRefused", "UserTask", nullptr);

    CHECK_THROWS_AS(facade.reportIssue(999, "item1", "X", "Y", 1), std::invalid_argument);

    facade.cancelWorkflow(id);
    CHECK_THROWS_AS(facade.reportIssue(id, "item1", "X", "Y", 1), std::logic_error);
}