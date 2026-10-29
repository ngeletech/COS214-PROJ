#include "doctest.h"
#include <stdexcept>
#include <string>
#include "InstanceSnapshot.h"
#include "State.h"
#include "WorkData.h"
#include "composite.h"
#include "WorkflowEngine.h"

// TEMPORARY: WorkflowInstance::restore() calls this, but src/WorkflowEngine.cpp
// doesn't compile on dev yet. Delete this once WorkflowEngine.cpp is in CMakeLists.txt.
Participant* WorkflowEngine::findParticipant(int) const {
    return nullptr;
}

namespace {
    // Minimal concrete leaf for tests (Task::doWork is pure virtual)
    class DummyTask : public Task {
    public:
        DummyTask(const std::string& n) : Task(n) {}
    protected:
        void doWork() override {}
    };
}
TEST_CASE("a new instance starts in the Created state") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    CHECK(instance.getStateName() == "Created");
}

TEST_CASE("normal lifecycle: start, suspend, resume, complete") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    instance.start();
    CHECK(instance.getStateName() == "Running");
    instance.suspend();
    CHECK(instance.getStateName() == "Suspended");
    instance.resume();
    CHECK(instance.getStateName() == "Running");
    instance.complete();
    CHECK(instance.getStateName() == "Completed");
}

TEST_CASE("illegal transitions throw std::logic_error") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    CHECK_THROWS_AS(instance.suspend(), std::logic_error);
    CHECK_THROWS_AS(instance.resume(), std::logic_error);
    CHECK_THROWS_AS(instance.complete(), std::logic_error);

    instance.start();
    CHECK_THROWS_AS(instance.start(), std::logic_error);
    CHECK_THROWS_AS(instance.resume(), std::logic_error);
}

TEST_CASE("cancel works from Created, Running and Suspended") {
    WorkflowInstance created(1, nullptr, WorkData(), nullptr);
    created.cancel();
    CHECK(created.getStateName() == "Cancelled");

    WorkflowInstance running(2, nullptr, WorkData(), nullptr);
    running.start();
    running.cancel();
    CHECK(running.getStateName() == "Cancelled");

    WorkflowInstance suspended(3, nullptr, WorkData(), nullptr);
    suspended.start();
    suspended.suspend();
    suspended.cancel();
    CHECK(suspended.getStateName() == "Cancelled");
}

TEST_CASE("terminal states refuse every action") {
    WorkflowInstance done(1, nullptr, WorkData(), nullptr);
    done.start();
    done.complete();
    CHECK_THROWS_AS(done.start(), std::logic_error);
    CHECK_THROWS_AS(done.suspend(), std::logic_error);
    CHECK_THROWS_AS(done.cancel(), std::logic_error);

    WorkflowInstance cancelled(2, nullptr, WorkData(), nullptr);
    cancelled.cancel();
    CHECK_THROWS_AS(cancelled.start(), std::logic_error);
    CHECK_THROWS_AS(cancelled.cancel(), std::logic_error);
}

TEST_CASE("work tree helpers are safe when there is no root") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    CHECK(instance.findElement("anything") == nullptr);
    CHECK_NOTHROW(instance.cancelOutstandingWork());
    CHECK_NOTHROW(instance.cancel());
}

TEST_CASE("restore with no root still restores data and state") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    instance.setData("coupon", "SAVE10");
    instance.start();
    InstanceSnapshot snap = instance.saveSnapshot();

    instance.setData("coupon", "SAVE50");
    instance.suspend();

    CHECK_NOTHROW(instance.restore(snap));
    CHECK(instance.getStateName() == "Running");
    CHECK(instance.getData().getData().at("coupon") == "SAVE10");
}

TEST_CASE("findElement finds elements at any depth and returns nullptr otherwise") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    WorkGroup* root = new WorkGroup("root");
    DummyTask* top = new DummyTask("top");
    SequentialGroup* inner = new SequentialGroup("inner");
    DummyTask* nested = new DummyTask("nested");

    root->add(top);
    root->add(inner);
    inner->add(nested);
    instance.setRoot(root);           // instance now owns the tree

    CHECK(instance.findElement("top") == top);
    CHECK(instance.findElement("nested") == nested);
    CHECK(instance.findElement("inner") == inner);
    CHECK(instance.findElement("missing") == nullptr);
}

TEST_CASE("cancelOutstandingWork cancels unfinished leaves only") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    WorkGroup* root = new WorkGroup("root");
    DummyTask* waiting = new DummyTask("waiting");
    DummyTask* started = new DummyTask("started");
    DummyTask* done = new DummyTask("done");
    DummyTask* rejected = new DummyTask("rejected");

    waiting->setStatus(WorkItemStatus::AVAILABLE);
    started->setStatus(WorkItemStatus::STARTED);
    done->setStatus(WorkItemStatus::COMPLETED);
    rejected->setStatus(WorkItemStatus::REJECTED);

    root->add(waiting);
    root->add(started);
    root->add(done);
    root->add(rejected);
    instance.setRoot(root);

    instance.cancelOutstandingWork();

    CHECK((waiting->getStatus() == WorkItemStatus::CANCELLED));
    CHECK((started->getStatus() == WorkItemStatus::CANCELLED));
    CHECK((done->getStatus() == WorkItemStatus::COMPLETED));
    CHECK((rejected->getStatus() == WorkItemStatus::REJECTED));
}

TEST_CASE("cancelling a running instance cancels its outstanding work") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    WorkGroup* root = new WorkGroup("root");
    DummyTask* task = new DummyTask("task");
    task->setStatus(WorkItemStatus::STARTED);
    root->add(task);
    instance.setRoot(root);

    instance.start();
    instance.cancel();

    CHECK(instance.getStateName() == "Cancelled");
    CHECK((task->getStatus() == WorkItemStatus::CANCELLED));
}

TEST_CASE("restore puts element statuses back") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    WorkGroup* root = new WorkGroup("root");
    DummyTask* task = new DummyTask("task");
    task->setStatus(WorkItemStatus::AVAILABLE);
    root->add(task);
    instance.setRoot(root);

    InstanceSnapshot snap = instance.saveSnapshot();
    task->setStatus(WorkItemStatus::COMPLETED);

    instance.restore(snap);
    CHECK((task->getStatus() == WorkItemStatus::AVAILABLE));
}