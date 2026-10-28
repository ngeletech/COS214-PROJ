#include "doctest.h"
#include "State.h"
#include "composite.h"
#include "InstanceSnapshot.h"
#include "Observer.h"
#include "WorkflowEvent.h"
#include "Issue.h"
#include <type_traits>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Records the name of every event it receives.
class RecordingListener : public WorkflowListener {
public:
    std::vector<std::string> events;
    void update(const WorkflowEvent& e) override {
        events.push_back(e.getEventName());
    }
};

// Builds an instance with no definition and no engine.
WorkflowInstance* makeInstance(int id = 1) {
    return new WorkflowInstance(id, nullptr, WorkData(), nullptr);
}

// Builds a valid Issue (Issue has no default constructor).
Issue makeIssue() {
    return Issue("Test", "Test issue", 1, "element");
}

} // namespace

// ---------------- your original test ----------------

TEST_CASE("State classes derive from InstanceState") {
    CHECK(std::is_base_of<InstanceState, CreatedState>::value);
    CHECK(std::is_base_of<InstanceState, RunningState>::value);
    CHECK(std::is_base_of<InstanceState, SuspendedState>::value);
    CHECK(std::is_base_of<InstanceState, CompletedState>::value);
    CHECK(std::is_base_of<InstanceState, FailedState>::value);
    CHECK(std::is_base_of<InstanceState, CancelledState>::value);
}

// ---------------- state names and factory ----------------

TEST_CASE("Concrete states report their names") {
    WorkflowInstance* inst = makeInstance();
    CreatedState created(*inst);
    RunningState running(*inst);
    SuspendedState suspended(*inst);
    CompletedState completed(*inst);
    FailedState failed(*inst);
    CancelledState cancelled(*inst);

    CHECK(created.getName() == "Created");
    CHECK(running.getName() == "Running");
    CHECK(suspended.getName() == "Suspended");
    CHECK(completed.getName() == "Completed");
    CHECK(failed.getName() == "Failed");
    CHECK(cancelled.getName() == "Cancelled");
    delete inst;
}

TEST_CASE("canCancel is true only where cancelling is allowed") {
    WorkflowInstance* inst = makeInstance();
    CHECK(CreatedState(*inst).canCancel());
    CHECK(RunningState(*inst).canCancel());
    CHECK(SuspendedState(*inst).canCancel());
    CHECK_FALSE(CompletedState(*inst).canCancel());
    CHECK_FALSE(FailedState(*inst).canCancel());
    CHECK_FALSE(CancelledState(*inst).canCancel());
    delete inst;
}

TEST_CASE("InstanceState::create builds the right state") {
    WorkflowInstance* inst = makeInstance();
    const char* names[] = {"Created", "Running", "Suspended",
                           "Completed", "Failed", "Cancelled"};
    for (int k = 0; k < 6; ++k) {
        InstanceState* s = InstanceState::create(names[k], *inst);
        REQUIRE(s != nullptr);
        CHECK(s->getName() == names[k]);
        delete s;
    }
    CHECK_THROWS_AS(InstanceState::create("Bogus", *inst), std::invalid_argument);
    delete inst;
}

// ---------------- lifecycle ----------------

TEST_CASE("New instance starts in Created") {
    WorkflowInstance* inst = makeInstance(7);
    CHECK(inst->getId() == 7);
    CHECK(inst->getStateName() == "Created");
    delete inst;
}

TEST_CASE("Normal lifecycle: Created -> Running -> Suspended -> Running -> Completed") {
    WorkflowInstance* inst = makeInstance();
    inst->start();
    CHECK(inst->getStateName() == "Running");
    inst->suspend();
    CHECK(inst->getStateName() == "Suspended");
    inst->resume();
    CHECK(inst->getStateName() == "Running");
    inst->complete();
    CHECK(inst->getStateName() == "Completed");
    delete inst;
}

TEST_CASE("Cancel is allowed from Created, Running and Suspended") {
    WorkflowInstance* a = makeInstance();
    a->cancel();
    CHECK(a->getStateName() == "Cancelled");

    WorkflowInstance* b = makeInstance();
    b->start();
    b->cancel();
    CHECK(b->getStateName() == "Cancelled");

    WorkflowInstance* c = makeInstance();
    c->start();
    c->suspend();
    c->cancel();
    CHECK(c->getStateName() == "Cancelled");

    delete a; delete b; delete c;
}

TEST_CASE("Fail moves a running instance to Failed") {
    WorkflowInstance* inst = makeInstance();
    inst->start();
    inst->fail(makeIssue());       // adjust if Issue needs constructor arguments
    CHECK(inst->getStateName() == "Failed");
    delete inst;
}

// ---------------- illegal transitions ----------------

TEST_CASE("Illegal transitions from Created throw") {
    WorkflowInstance* inst = makeInstance();
    CHECK_THROWS_AS(inst->suspend(), std::logic_error);
    CHECK_THROWS_AS(inst->resume(), std::logic_error);
    CHECK_THROWS_AS(inst->complete(), std::logic_error);
    CHECK_THROWS_AS(inst->fail(makeIssue)), std::logic_error);
    CHECK(inst->getStateName() == "Created");   // state unchanged
    delete inst;
}

TEST_CASE("Illegal transitions from Running and Suspended throw") {
    WorkflowInstance* inst = makeInstance();
    inst->start();
    CHECK_THROWS_AS(inst->start(), std::logic_error);
    CHECK_THROWS_AS(inst->resume(), std::logic_error);

    inst->suspend();
    CHECK_THROWS_AS(inst->suspend(), std::logic_error);
    CHECK_THROWS_AS(inst->complete(), std::logic_error);
    CHECK_THROWS_AS(inst->fail(makeIssue()), std::logic_error);
    CHECK(inst->getStateName() == "Suspended");
    delete inst;
}

TEST_CASE("Terminal states refuse every action") {
    WorkflowInstance* done = makeInstance();
    done->start();
    done->complete();
    CHECK_THROWS_AS(done->start(), std::logic_error);
    CHECK_THROWS_AS(done->suspend(), std::logic_error);
    CHECK_THROWS_AS(done->resume(), std::logic_error);
    CHECK_THROWS_AS(done->complete(), std::logic_error);
    CHECK_THROWS_AS(done->cancel(), std::logic_error);
    CHECK_THROWS_AS(done->fail(makeIssue()), std::logic_error);

    WorkflowInstance* gone = makeInstance();
    gone->cancel();
    CHECK_THROWS_AS(gone->start(), std::logic_error);
    CHECK_THROWS_AS(gone->cancel(), std::logic_error);

    delete done; delete gone;
}

// ---------------- observer ----------------

TEST_CASE("setData stores the value and notifies subscribers") {
    WorkflowInstance* inst = makeInstance();
    RecordingListener l;
    inst->subscribe(&l);
    inst->subscribe(nullptr);                  // ignored, must not crash

    inst->setData("k", "v");
    CHECK(inst->getData().getData().at("k") == "v");
    REQUIRE(l.events.size() == 1);
    CHECK(l.events[0] == "DataChanged");
    delete inst;
}

TEST_CASE("publish reaches every subscriber") {
    WorkflowInstance* inst = makeInstance();
    RecordingListener a, b;
    inst->subscribe(&a);
    inst->subscribe(&b);
    inst->publish("Ping");
    CHECK(a.events.size() == 1);
    CHECK(b.events.size() == 1);
    delete inst;
}

// ---------------- memento ----------------

TEST_CASE("Snapshot restores state and data") {
    WorkflowInstance* inst = makeInstance();
    inst->setRoot(new WorkGroup("root"));
    inst->setData("k", "before");
    inst->start();

    InstanceSnapshot snap = inst->saveSnapshot();

    inst->suspend();
    inst->setData("k", "after");
    CHECK(inst->getStateName() == "Suspended");

    inst->restore(snap);
    CHECK(inst->getStateName() == "Running");
    CHECK(inst->getData().getData().at("k") == "before");
    delete inst;
}

TEST_CASE("setRoot replaces the old root without crashing") {
    WorkflowInstance* inst = makeInstance();
    WorkGroup* first = new WorkGroup("first");
    inst->setRoot(first);
    CHECK(inst->getRoot() == first);
    inst->setRoot(first);                      // same pointer: no double delete
    WorkGroup* second = new WorkGroup("second");
    inst->setRoot(second);
    CHECK(inst->getRoot() == second);
    delete inst;
}