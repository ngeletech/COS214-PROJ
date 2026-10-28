/**
 * @file test_builder.cpp
 * @brief Unit tests for the Builder design pattern.
 */

#include "doctest.h"
#include "../include/Builder.h"

    TEST_CASE("WorkflowDesigner - Assembly via Director") {
        WorkflowDefinitionBuilder* builder = new StandardDefinitionBuilder();
        WorkflowDesigner designer(builder);

        std::vector<WorkItemSpec> specs = {
            {"Task", "Step1", "Config1"}
        };

        // Use nullptr while Strategy .cpp is not yet linked
        AssignmentStrategy* strategy = nullptr;
        EscalationHandler* handler = nullptr; 

        WorkflowDefinition* def = designer.construct("MainWorkflow", specs, strategy, handler);

        CHECK(def != nullptr);
        CHECK(def->getName() == "MainWorkflow");
        CHECK(def->getWorkItemSpecs().size() == 1);

        delete def; 
    }

    TEST_CASE("StandardDefinitionBuilder - Empty Workflow Throws Exception") {
        WorkflowDefinitionBuilder* builder = new StandardDefinitionBuilder();
        builder->setName("Empty Plan");

        CHECK_THROWS_AS(builder->build(), std::runtime_error);
        delete builder;
    }

    TEST_CASE("WorkflowDesigner - Assembly via Director") {
        WorkflowDefinitionBuilder* builder = new StandardDefinitionBuilder();
        WorkflowDesigner designer(builder);

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
