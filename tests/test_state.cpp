#include "doctest.h"
#include "State.h"
#include "composite.h"
#include "InstanceSnapshot.h"
#include "WorkflowEngine.h"
#include "Issue.h"
#include <stdexcept>
#include <string>

// TEMPORARY: WorkflowInstance::restore() calls this, but src/WorkflowEngine.cpp
// doesn't compile on dev yet. Delete once WorkflowEngine.cpp is in CMakeLists.txt.
Participant* WorkflowEngine::findParticipant(int) const {
    return nullptr;
}

namespace {

// Minimal concrete leaf (Task::doWork is pure virtual).
class DummyTask : public Task {
public:
    DummyTask(const std::string& n) : Task(n) {}
protected:
    void doWork() override {}
};

Issue makeIssue() {
    return Issue("Test", "Test issue", 1, "element");
}

} // namespace

TEST_CASE("Lifecycle: Created -> Running -> Suspended -> Running -> Completed") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    CHECK(instance.getStateName() == "Created");

    instance.start();
    CHECK(instance.getStateName() == "Running");
    instance.suspend();
    CHECK(instance.getStateName() == "Suspended");
    instance.resume();
    CHECK(instance.getStateName() == "Running");
    instance.complete();
    CHECK(instance.getStateName() == "Completed");
}

TEST_CASE("Cancel works from Running, fail works, and illegal actions throw") {
    WorkflowInstance running(1, nullptr, WorkData(), nullptr);
    running.start();
    running.cancel();
    CHECK(running.getStateName() == "Cancelled");
    CHECK_THROWS_AS(running.start(), std::logic_error);   // terminal state

    WorkflowInstance failing(2, nullptr, WorkData(), nullptr);
    CHECK_THROWS_AS(failing.complete(), std::logic_error); // illegal from Created
    failing.start();
    failing.fail(makeIssue());
    CHECK(failing.getStateName() == "Failed");
}

TEST_CASE("Snapshot restores state and data") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    instance.setData("k", "before");
    instance.start();
    InstanceSnapshot snap = instance.saveSnapshot();

    instance.suspend();
    instance.setData("k", "after");

    instance.restore(snap);
    CHECK(instance.getStateName() == "Running");
    CHECK(instance.getData().getData().at("k") == "before");
}

TEST_CASE("findElement finds elements by name, nullptr otherwise") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    CHECK(instance.findElement("anything") == nullptr);   // no root yet

    WorkGroup* root = new WorkGroup("root");
    DummyTask* nested = new DummyTask("nested");
    SequentialGroup* inner = new SequentialGroup("inner");
    root->add(inner);
    inner->add(nested);