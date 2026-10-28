#include "doctest.h"
#include "composite.h"
#include "Participant.h"
#include "TemplateMethod.h"
#include "WorkData.h"
#include "State.h"

#include <string>
#include <vector>

namespace {

class RecordingElement : public WorkElement {
private:
    std::vector<std::string>* executed;

public:
    RecordingElement(const std::string& name, std::vector<std::string>* executed) : WorkElement(name) {
        this->executed = executed;
    }

    void execute() override {
        executed->push_back(getName());
    }

    void complete() override {
        setStatus(WorkItemStatus::COMPLETED);
        if(parent != nullptr){
            parent->complete();
        }
    }

    bool isComplete() const override {
        return getStatus() == WorkItemStatus::COMPLETED;
    }
};

}

TEST_CASE("A sequential group unlocks its children one at a time") {
    SequentialGroup root("root");
    UserTask* joel = new UserTask("Joel");
    UserTask* ellie = new UserTask("Ellie");
    UserTask* tommy = new UserTask("Tommy");
    root.add(joel);
    root.add(ellie);
    root.add(tommy);

    CHECK(joel->isAvailable());
    CHECK_FALSE(ellie->isAvailable());
    CHECK_FALSE(tommy->isAvailable());

    joel->complete();
    CHECK(ellie->isAvailable());
    CHECK_FALSE(tommy->isAvailable());

    ellie->complete();
    CHECK(tommy->isAvailable());
}

TEST_CASE("Completion moves up the tree") {
    SequentialGroup root("root");
    UserTask* joel = new UserTask("Joel");
    ParallelGroup* group = new ParallelGroup("Tess");
    UserTask* bill = new UserTask("Bill");
    UserTask* frank = new UserTask("Frank");
    root.add(joel);
    root.add(group);
    group->add(bill);
    group->add(frank);

    joel->complete();
    bill->complete();
    CHECK_FALSE(group->isComplete());
    CHECK(group->getStatus() == WorkItemStatus::CREATED);
    CHECK(root.getStatus() == WorkItemStatus::CREATED);

    frank->complete();
    CHECK(group->getStatus() == WorkItemStatus::COMPLETED);
    CHECK(root.getStatus() == WorkItemStatus::COMPLETED);
}

TEST_CASE("Availability checks every group above, not just the parent") {
    SequentialGroup root("root");
    UserTask* joel = new UserTask("Joel");
    ParallelGroup* group = new ParallelGroup("Tess");
    UserTask* bill = new UserTask("Bill");
    root.add(joel);
    root.add(group);
    group->add(bill);

    CHECK(group->canStartChild("Bill"));
    CHECK_FALSE(bill->isAvailable());

    joel->complete();
    CHECK(bill->isAvailable());
}

TEST_CASE("A parallel group unlocks all its children at once") {
    ParallelGroup group("Maria");
    UserTask* ellie = new UserTask("Ellie");
    UserTask* dina = new UserTask("Dina");
    group.add(ellie);
    group.add(dina);

    CHECK(ellie->isAvailable());
    CHECK(dina->isAvailable());
}

TEST_CASE("An empty group counts as complete") {
    ParallelGroup group("Riley");
    CHECK(group.isComplete());

    group.complete();
    CHECK(group.getStatus() == WorkItemStatus::COMPLETED);
}

TEST_CASE("A group never completes early") {
    SequentialGroup group("Sam");
    group.add(new UserTask("Henry"));

    group.complete();
    CHECK_FALSE(group.isComplete());
    CHECK(group.getStatus() == WorkItemStatus::CREATED);
}

TEST_CASE("A group is never available and reports itself as a group") {
    SequentialGroup group("Marlene");
    CHECK_FALSE(group.isAvailable());
    CHECK(group.asGroup() == &group);
}

TEST_CASE("canStartChild() with a name that isn't a child") {
    SequentialGroup sequential("Abby");
    ParallelGroup parallel("Lev");
    sequential.add(new UserTask("Owen"));
    parallel.add(new UserTask("Mel"));

    CHECK_FALSE(sequential.canStartChild("Nora"));
    CHECK(parallel.canStartChild("Nora"));
}

TEST_CASE("add() passes the group's instance down to the child") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    SequentialGroup group("Marlene");
    group.setInstance(&instance);

    UserTask* riley = new UserTask("Riley");
    group.add(riley);
    CHECK(riley->getInstance() == &instance);
}

TEST_CASE("execute() only runs the children that may start") {
    std::vector<std::string> executed;

    SUBCASE("a sequential group runs only its first unfinished child") {
        SequentialGroup group("Tommy");
        group.add(new RecordingElement("Joel", &executed));
        group.add(new RecordingElement("Ellie", &executed));

        group.execute();
        REQUIRE(executed.size() == 1);
        CHECK(executed[0] == "Joel");
    }

    SUBCASE("a parallel group runs every child") {
        ParallelGroup group("Tommy");
        group.add(new RecordingElement("Joel", &executed));
        group.add(new RecordingElement("Ellie", &executed));

        group.execute();
        REQUIRE(executed.size() == 2);
        CHECK(executed[0] == "Joel");
        CHECK(executed[1] == "Ellie");
    }
}

TEST_CASE("A new task starts with the defaults") {
    UserTask ellie("Ellie");

    CHECK(ellie.getName() == "Ellie");
    CHECK(ellie.getStatus() == WorkItemStatus::CREATED);
    CHECK(ellie.getInstance() == nullptr);
    CHECK(ellie.getAssignee() == nullptr);
    CHECK(ellie.getPriority() == 0);
    CHECK(ellie.asGroup() == nullptr);
    CHECK(ellie.requiresAssignee());
    CHECK_FALSE(ellie.isComplete());
}

TEST_CASE("assignTo() stores the participant without changing the status") {
    Participant joel(1, "Joel", "Smuggler");
    UserTask ellie("Ellie");

    ellie.assignTo(&joel);
    CHECK(ellie.getAssignee() == &joel);
    CHECK(ellie.getStatus() == WorkItemStatus::CREATED);
}

TEST_CASE("Completing an assigned task lowers the participant's workload") {
    Participant joel(1, "Joel", "Smuggler");
    UserTask ellie("Ellie");
    ellie.assignTo(&joel);
    joel.incrementWorkload(); // the engine does this on assignment
    REQUIRE(joel.getWorkload() == 1);

    ellie.complete();
    CHECK(ellie.isComplete());
    CHECK(joel.getWorkload() == 0);
}

TEST_CASE("getPayload() with no instance returns empty data") {
    UserTask ellie("Ellie");
    CHECK(ellie.getPayload().getData().empty());
}
