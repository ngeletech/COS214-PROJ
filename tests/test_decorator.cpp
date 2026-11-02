#include "doctest.h"
#include "TemplateMethod.h"          
#include "Decorator.h"
#include "composite.h"
#include "WorkData.h"


TEST_CASE("Decorator forwards basic properties") {
    WorkElement* task      = new UserTask("my-task");
    WorkElement* decorated = new AuditedDecorator(task);

    CHECK(decorated->getName() == "my-task");
    delete decorated;   
}

TEST_CASE("PriorityDecorator overrides priority") {
    WorkElement* task = new UserTask("task");
    WorkElement* prio = new PriorityDecorator(task, 7);

    CHECK(prio->getPriority() == 7);
    CHECK(prio->getName() == "task");
    delete prio;
}

TEST_CASE("PayloadDecorator adds data") {
    WorkElement* task    = new UserTask("task");
    WorkElement* payload = new PayloadDecorator(task, "env", "prod");

    WorkData d = payload->getPayload();
    CHECK(d.getData().at("env") == "prod");
    delete payload;
}

TEST_CASE("Audited + Priority stack") {
    // Chain: PriorityDecorator( AuditedDecorator( UserTask ) )
    WorkElement* task = new UserTask("stacked");
    task = new AuditedDecorator(task);
    task = new PriorityDecorator(task, 3);

    CHECK(task->getName() == "stacked");
    CHECK(task->getPriority() == 3);
    delete task;
}