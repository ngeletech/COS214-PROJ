#include "doctest.h"
#include "composite.h"
#include "Iterator.h"
#include "TemplateMethod.h"
#include "WorkData.h"
#include "State.h"

#include <string>
#include <vector>

namespace {

std::vector<std::string> namesFrom(WorkIterator<WorkElement>& it){
    std::vector<std::string> names;
    for(it.first(); !it.isDone(); it.next()){
        names.push_back(it.current()->getName());
    }
    return names;
}

}

TEST_CASE("DepthFirstIterator visits a group before its children, in the order they were added") {
    SequentialGroup root("root");
    ParallelGroup* group = new ParallelGroup("Tommy");
    root.add(new UserTask("Joel"));
    root.add(group);
    group->add(new UserTask("Maria"));
    group->add(new UserTask("Jesse"));
    root.add(new UserTask("Ellie"));

    DepthFirstIterator it(&root);
    std::vector<std::string> expected = {"root", "Joel", "Tommy", "Maria", "Jesse", "Ellie"};
    CHECK(namesFrom(it) == expected);
}

TEST_CASE("DepthFirstIterator is on the root straight after it's created") {
    SequentialGroup root("root");
    root.add(new UserTask("Joel"));

    DepthFirstIterator it(&root);
    CHECK_FALSE(it.isDone());
    CHECK(it.current() == &root);
}

TEST_CASE("first() restarts the walk") {
    SequentialGroup root("root");
    root.add(new UserTask("Joel"));
    root.add(new UserTask("Ellie"));

    DepthFirstIterator it(&root);
    std::vector<std::string> firstWalk = namesFrom(it);
    std::vector<std::string> secondWalk = namesFrom(it);
    CHECK(firstWalk.size() == 3);
    CHECK(firstWalk == secondWalk);
}

TEST_CASE("createIterator() on an inner group walks only that group") {
    SequentialGroup root("root");
    ParallelGroup* group = new ParallelGroup("Abby");
    root.add(new UserTask("Ellie"));
    root.add(group);
    group->add(new UserTask("Lev"));
    group->add(new UserTask("Yara"));

    WorkIterator<WorkElement>* it = group->createIterator();
    std::vector<std::string> expected = {"Abby", "Lev", "Yara"};
    CHECK(namesFrom(*it) == expected);
    delete it;
}

TEST_CASE("DepthFirstIterator edge cases") {
    SUBCASE("a single task as the root") {
        UserTask joel("Joel");
        DepthFirstIterator it(&joel);
        std::vector<std::string> expected = {"Joel"};
        CHECK(namesFrom(it) == expected);
    }

    SUBCASE("an empty group as the root") {
        ParallelGroup group("Riley");
        DepthFirstIterator it(&group);
        std::vector<std::string> expected = {"Riley"};
        CHECK(namesFrom(it) == expected);
    }

    SUBCASE("a null root is done straight away") {
        DepthFirstIterator it(nullptr);
        CHECK(it.isDone());
        CHECK(it.current() == nullptr);
    }
}

TEST_CASE("AvailableItemIterator only returns what can be unlocked now") {
    SequentialGroup root("root");
    UserTask* joel = new UserTask("Joel");
    ParallelGroup* group = new ParallelGroup("Tommy");
    UserTask* maria = new UserTask("Maria");
    UserTask* jesse = new UserTask("Jesse");
    UserTask* ellie = new UserTask("Ellie");
    root.add(joel);
    root.add(group);
    group->add(maria);
    group->add(jesse);
    root.add(ellie);

    AvailableItemIterator it(&root);
    std::vector<std::string> onlyJoel = {"Joel"};
    std::vector<std::string> mariaAndJesse = {"Maria", "Jesse"};
    std::vector<std::string> onlyJesse = {"Jesse"};
    std::vector<std::string> onlyEllie = {"Ellie"};

    CHECK(namesFrom(it) == onlyJoel);

    joel->complete();
    CHECK(namesFrom(it) == mariaAndJesse);

    maria->complete();
    CHECK(namesFrom(it) == onlyJesse);

    jesse->complete();
    CHECK(namesFrom(it) == onlyEllie);

    ellie->complete();
    CHECK(namesFrom(it).empty());
}

TEST_CASE("AvailableItemIterator skips groups and tasks that are no longer CREATED") {
    ParallelGroup root("root");
    UserTask* joel = new UserTask("Joel");
    UserTask* ellie = new UserTask("Ellie");
    root.add(joel);
    root.add(ellie);

    joel->setStatus(WorkItemStatus::AVAILABLE);

    AvailableItemIterator it(&root);
    std::vector<std::string> expected = {"Ellie"};
    CHECK(namesFrom(it) == expected);
}

TEST_CASE("AuditEntry returns what it was given") {
    AuditEntry entry(3, "ElementCompleted", "Ellie");
    CHECK(entry.getInstanceId() == 3);
    CHECK(entry.getEventType() == "ElementCompleted");
    CHECK(entry.getDetails() == "Ellie");
}

TEST_CASE("AuditEntryIterator walks the entries in order") {
    std::vector<AuditEntry> entries;
    entries.push_back(AuditEntry(1, "InstanceStarted", ""));
    entries.push_back(AuditEntry(1, "ElementCompleted", "Joel"));
    AuditEntryIterator it(&entries);

    SUBCASE("it visits each entry in order") {
        REQUIRE_FALSE(it.isDone());
        CHECK(it.current()->getEventType() == "InstanceStarted");
        it.next();
        REQUIRE_FALSE(it.isDone());
        CHECK(it.current()->getEventType() == "ElementCompleted");
        it.next();
        CHECK(it.isDone());
    }

    SUBCASE("stepping past the end leaves it done") {
        it.next();
        it.next();
        it.next();
        CHECK(it.isDone());
        CHECK(it.current() == nullptr);
    }

    SUBCASE("an empty log is done straight away") {
        std::vector<AuditEntry> none;
        AuditEntryIterator empty(&none);
        CHECK(empty.isDone());
        CHECK(empty.current() == nullptr);
    }
}

TEST_CASE("AuditHistory keeps entries in the order they were added") {
    AuditHistory history;
    history.append(AuditEntry(1, "InstanceStarted", ""));
    history.append(AuditEntry(1, "ElementCompleted", "Joel"));

    std::vector<std::string> types;
    WorkIterator<const AuditEntry>* it = history.createIterator();
    for(it->first(); !it->isDone(); it->next()){
        types.push_back(it->current()->getEventType());
    }
    delete it;

    std::vector<std::string> expected = {"InstanceStarted", "ElementCompleted"};
    CHECK(types == expected);
}

TEST_CASE("An AuditHistory iterator sees entries appended after it was created") {
    AuditHistory history;
    WorkIterator<const AuditEntry>* it = history.createIterator();
    CHECK(it->isDone());

    history.append(AuditEntry(1, "InstanceStarted", ""));
    it->first();
    CHECK_FALSE(it->isDone());
    delete it;
}

TEST_CASE("auditReport() lists only the requested instance's entries") {
    AuditHistory history;
    history.append(AuditEntry(1, "InstanceStarted", ""));
    history.append(AuditEntry(2, "InstanceStarted", ""));
    history.append(AuditEntry(1, "ElementCompleted", "Ellie"));
    ReportGenerator reports(&history);

    CHECK(reports.auditReport(1) == "InstanceStarted\nElementCompleted: Ellie\n");
    CHECK(reports.auditReport(2) == "InstanceStarted\n");
    CHECK(reports.auditReport(99) == "");
}

TEST_CASE("auditReport() with no audit log is empty") {
    ReportGenerator reports(nullptr);
    CHECK(reports.auditReport(1) == "");
}

// skipped until WorkflowInstance's constructor creates the state and the root group
TEST_CASE("progressReport() counts the finished tasks" * doctest::skip()) {
    WorkflowInstance instance(3, nullptr, WorkData(), nullptr);
    WorkGroup* root = instance.getRoot();
    REQUIRE(root != nullptr);

    UserTask* joel = new UserTask("Joel");
    ParallelGroup* group = new ParallelGroup("Tommy");
    UserTask* maria = new UserTask("Maria");
    root->add(joel);
    root->add(group);
    group->add(maria);
    group->add(new UserTask("Jesse"));
    root->add(new UserTask("Ellie"));

    joel->complete();
    maria->complete();

    ReportGenerator reports(nullptr);
    CHECK(reports.progressReport(instance) == "Instance 3: Created, 2/4 tasks complete");
}
