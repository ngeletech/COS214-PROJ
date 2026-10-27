/**
 * @file test_builder.cpp
 * @brief Unit tests for the Builder design pattern.
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../include/Builder.h"

TEST_SUITE("Builder Pattern Tests") {

    TEST_CASE("StandardDefinitionBuilder - Construct Valid Workflow") {
        StandardDefinitionBuilder builder;
        builder.setName("Meal Order Workflow");

        WorkItemSpec step1{"Task", "PrepBreakfast", "StandardMenu"};
        WorkItemSpec step2{"Task", "PackageOrder", "EcoBox"};

        builder.addWorkItem(step1);
        builder.addWorkItem(step2);

        WorkflowDefinition* def = builder.build();

        REQUIRE(def != nullptr);
        CHECK(def->getName() == "Meal Order Workflow");
        CHECK(def->getWorkItemSpecs().size() == 2);
        CHECK(def->getWorkItemSpecs()[0].name == "PrepBreakfast");

        delete def;
    }

    TEST_CASE("StandardDefinitionBuilder - Empty Workflow Throws Exception") {
        StandardDefinitionBuilder builder;
        builder.setName("Empty Plan");

        CHECK_THROWS_AS(builder.build(), std::runtime_error);
    }

    TEST_CASE("WorkflowDesigner - Assembly via Director") {
        StandardDefinitionBuilder builder;
        WorkflowDesigner designer(&builder);

        std::vector<WorkItemSpec> specs = {
            {"Task", "PrepBreakfast", "MenuA"},
            {"Task", "DispatchOrder", "Courier1"}
        };

        WorkflowDefinition* def = designer.construct("Director Pipeline", specs, nullptr, nullptr);

        REQUIRE(def != nullptr);
        CHECK(def->getName() == "Director Pipeline");
        CHECK(def->getWorkItemSpecs().size() == 2);

        delete def;
    }
}