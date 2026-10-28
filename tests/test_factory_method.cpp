#include "doctest.h"
#include <stdexcept>
#include <string>
#include "WorkItemFactory.h"
#include "Builder.h"
#include "WorkData.h"
#include "composite.h"
#include "TemplateMethod.h"

namespace {
    WorkItemSpec makeSpec (const std::string& type, const std::string& name, const std::string& config = ""){
        WorkItemSpec spec;
        spec.type = type;
        spec.name = name;
        spec.config = config;
        return spec;
    }
}

TEST_CASE("Each concrete factory builds the matching element type") {
    SUBCASE("UserTaskFactory") {
        UserTaskFactory factory;
        WorkElement* e = factory.create(makeSpec("UserTask", "u"), nullptr);
        REQUIRE(e != nullptr);
        CHECK(dynamic_cast<UserTask*>(e) != nullptr);
        delete e;
    }
    SUBCASE("ServiceTaskFactory") {
        ServiceTaskFactory factory;
        WorkElement* e = factory.create(makeSpec("ServiceTask", "s"), nullptr);
        REQUIRE(e != nullptr);
        CHECK(dynamic_cast<ServiceTask*>(e) != nullptr);
        delete e;
    }
    SUBCASE("ApprovalTaskFactory") {
        ApprovalTaskFactory factory;
        WorkElement* e = factory.create(makeSpec("ApprovalTask", "a"), nullptr);
        REQUIRE(e != nullptr);
        CHECK(dynamic_cast<ApprovalTask*>(e) != nullptr);
        delete e;
    }
    SUBCASE("ParallelGroupFactory") {
        ParallelGroupFactory factory;
        WorkElement* e = factory.create(makeSpec("ParallelGroup", "p"), nullptr);
        REQUIRE(e != nullptr);
        CHECK(dynamic_cast<ParallelGroup*>(e) != nullptr);
        delete e;
    }
    SUBCASE("SequentialGroupFactory") {
        SequentialGroupFactory factory;
        WorkElement* e = factory.create(makeSpec("SequentialGroup", "q"), nullptr);
        REQUIRE(e != nullptr);
        CHECK(dynamic_cast<SequentialGroup*>(e) != nullptr);
        delete e;
    }
}

TEST_CASE("create() keeps the spec name and sets the CREATED status") {
    UserTaskFactory factory;
    WorkElement* e = factory.create(makeSpec("UserTask", "review form"), nullptr);
    REQUIRE(e != nullptr);
    CHECK(e->getName() == "review form");
    CHECK(e->getStatus() == WorkItemStatus::CREATED);
    delete e;
}

TEST_CASE("ScriptTaskFactory reads key and value from the config") {
    ScriptTaskFactory factory;

    SUBCASE("builds a ScriptTask when both are present") {
        WorkElement* e = factory.create(makeSpec("ScriptTask", "UpdateOrderStatus", "OrderStatus, IN_PROGRESS"), nullptr);
        REQUIRE(e != nullptr);
        CHECK(dynamic_cast<ScriptTask*>(e) != nullptr);
        CHECK(e->getName() == "UpdateOrderStatus");
        delete e;
    }
    SUBCASE("also accepts no space after the comma") {
        WorkElement* e = factory.create(makeSpec("ScriptTask", "UpdateOrderStatus", "OrderStatus,IN_PROGRESS"), nullptr);
        CHECK(dynamic_cast<ScriptTask*>(e) != nullptr);
        delete e;
    }
    SUBCASE("throws when key and value are missing") {
        CHECK_THROWS_AS(factory.create(makeSpec("ScriptTask", "bad"), nullptr), std::invalid_argument);
    }
    SUBCASE("throws when the key is empty") {
        CHECK_THROWS_AS(factory.create(makeSpec("ScriptTask", "nokey", ",IN_PROGRESS"), nullptr), std::invalid_argument);
    }
}

TEST_CASE("Registry has the six standard factories") {
    WorkItemFactoryRegistry registry;
    const char* types[] = {"UserTask", "ServiceTask", "ApprovalTask", "ScriptTask", "ParallelGroup", "SequentialGroup"};

    for (const char* type : types) {
        CHECK_MESSAGE(registry.getFactory(type) != nullptr, type);
    }
}

TEST_CASE("Registry returns nullptr for an unknown type") {
    WorkItemFactoryRegistry registry;
    CHECK(registry.getFactory("NoSuchType") == nullptr);
}

TEST_CASE("Registry registers new types and replaces existing ones") {
    WorkItemFactoryRegistry registry;

    SUBCASE("a new type added") {
        registry.registerFactory("Custom", new UserTaskFactory());
        CHECK(registry.getFactory("Custom") != nullptr);
    }
    SUBCASE("an existing type is replaced") {
        WorkItemFactory* replacement = new ServiceTaskFactory();
        registry.registerFactory("UserTask", replacement);
        CHECK(registry.getFactory("UserTask") == replacement);

        WorkElement* e = registry.getFactory("UserTask")->create(makeSpec("UserTask", "x"), nullptr);
        CHECK(dynamic_cast<ServiceTask*> (e) != nullptr);
        delete e;
    }
    SUBCASE("registering the same pointer again is safe") {
        WorkItemFactory* same = registry.getFactory("UserTask");
        registry.registerFactory("UserTask", same);
        CHECK(registry.getFactory("UserTask") == same);
    }
}

TEST_CASE("Looking a factory up by spec type builds the right element") {
    WorkItemFactoryRegistry registry;
    WorkItemSpec spec = makeSpec("SequentialGroup", "stage 1");

    WorkItemFactory* factory = registry.getFactory(spec.type);
    REQUIRE(factory != nullptr);

    WorkElement* e = factory->create(spec, nullptr);
    CHECK(dynamic_cast<SequentialGroup*>(e) != nullptr);
    delete e;
}