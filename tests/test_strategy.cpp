
#include "doctest.h"
#include "AssignmentStrategy.h"
#include "Participant.h"
#include "WorkData.h"
#include "composite.h"

class TestTask : public Task {
public:
    TestTask(const std::string& name) : Task(name) {}

protected:
    void doWork() override {}
};

TEST_CASE("RoleMatchingStrategy selects a participant with the required role") {
    TestTask task("Review application");
    Participant alice(1, "Alice", "Developer");
    Participant bob(2, "Bob", "Manager");

    RoleMatchingStrategy strategy("Manager");
    WorkData data;
    std::vector<Participant*> candidates = {&alice, &bob};

    CHECK(strategy.select(task, data, candidates) == &bob);
}

TEST_CASE("RoleMatchingStrategy returns nullptr when no role matches") {
    TestTask task("Review application");
    Participant alice(1, "Alice", "Developer");

    RoleMatchingStrategy strategy("Manager");
    WorkData data;
    std::vector<Participant*> candidates = {&alice};

    CHECK(strategy.select(task, data, candidates) == nullptr);
}

TEST_CASE("AttributeMatchingStrategy selects a participant with the matching attribute") {
    TestTask task("Review application");
    Participant alice(1, "Alice", "Developer");
    Participant bob(2, "Bob", "Developer");

    alice.setAttribute("department", "Engineering");
    bob.setAttribute("department", "Finance");

    AttributeMatchingStrategy strategy("department", "Finance");
    WorkData data;
    std::vector<Participant*> candidates = {&alice, &bob};

    CHECK(strategy.select(task, data, candidates) == &bob);
}

TEST_CASE("AttributeMatchingStrategy returns nullptr when no attribute matches") {
    TestTask task("Review application");
    Participant alice(1, "Alice", "Developer");

    alice.setAttribute("department", "Engineering");

    AttributeMatchingStrategy strategy("department", "Finance");
    WorkData data;
    std::vector<Participant*> candidates = {&alice};

    CHECK(strategy.select(task, data, candidates) == nullptr);
}

TEST_CASE("LeastLoadedStrategy selects the participant with the lowest workload") {
    TestTask task("Review application");
    Participant alice(1, "Alice", "Developer");
    Participant bob(2, "Bob", "Developer");
    Participant charlie(3, "Charlie", "Developer");

    alice.incrementWorkload();
    alice.incrementWorkload();
    bob.incrementWorkload();

    LeastLoadedStrategy strategy;
    WorkData data;
    std::vector<Participant*> candidates = {&alice, &bob, &charlie};

    CHECK(strategy.select(task, data, candidates) == &charlie);
}

TEST_CASE("LeastLoadedStrategy returns nullptr for an empty candidate list") {
    TestTask task("Review application");

    LeastLoadedStrategy strategy;
    WorkData data;
    std::vector<Participant*> candidates;

    CHECK(strategy.select(task, data, candidates) == nullptr);
}

TEST_CASE("AssignmentService delegates selection to its strategy") {
    TestTask task("Review application");
    Participant alice(1, "Alice", "Developer");
    Participant bob(2, "Bob", "Manager");

    RoleMatchingStrategy strategy("Manager");
    AssignmentService service(&strategy);
    WorkData data;
    std::vector<Participant*> candidates = {&alice, &bob};

    CHECK(service.assignWorkItem(task, data, candidates) == &bob);
}
