#include "doctest.h"
#include <map>
#include <string>
#include "InstanceSnapshot.h"
#include "State.h"
#include "WorkData.h"

namespace {
    std::string read(WorkflowInstance& instance, const std::string& key) {
        std::map<std::string, std::string> values = instance.getData().getData();
        return values[key];
    }
}
TEST_CASE("canRollback is false when nothing has been saved") {
    RollbackManager manager;
    CHECK_FALSE(manager.canRollback(42));
}

TEST_CASE("save makes rollback possible") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    RollbackManager manager;

    instance.setData("coupon", "SAVE10");
    manager.save(&instance);
    instance.setData("coupon", "SAVE50");
    REQUIRE(read(instance, "coupon") == "SAVE50");

    manager.rollback(&instance);
    CHECK(read(instance, "coupon") == "SAVE10");
}

TEST_CASE("rollback uses up the snapshot") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    RollbackManager manager;
    manager.save(&instance);
    manager.rollback(&instance);
    CHECK_FALSE(manager.canRollback(1));
}

TEST_CASE("rollback with no snapshot changes nothing") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    RollbackManager manager;
    instance.setData("address", "12 Main Rd");
    manager.rollback(&instance);
    CHECK(read(instance, "address") == "12 Main Rd");
}

TEST_CASE("snapshots are restored latest first") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);
    RollbackManager manager;

    instance.setData("step", "one");
    manager.save(&instance);
    instance.setData("step", "two");
    manager.save(&instance);
    instance.setData("step", "three");

    manager.rollback(&instance);
    CHECK(read(instance, "step") ==  "two");
    manager.rollback(&instance);
    CHECK(read(instance, "step") == "one");
    CHECK_FALSE(manager.canRollback(1));
}

TEST_CASE("rolling back one instance does not touch another") {
    WorkflowInstance orderA(1, nullptr, WorkData(), nullptr);
    WorkflowInstance orderB(2, nullptr, WorkData(), nullptr);
    RollbackManager manager;

    orderA.setData("address", "A home");
    orderB.setData("address" , "B home");
    manager.save(&orderA);
    manager.save(&orderB);
    orderA.setData("address", "A new");
    orderB.setData("address", "B new");

    manager.rollback(&orderA);
    CHECK(read(orderA, "address") == "A home");
    CHECK(read(orderB, "address") == "B new");
    CHECK(manager.canRollback(2));

    manager.rollback(&orderB);
    CHECK(read(orderB, "address") == "B home");
}