/**
 * @file test_builder.cpp
 * @brief Unit tests for the Builder design pattern.
 */

#include "doctest.h"
#include "../include/Builder.h"
#include "../include/AssignmentStrategy.h"
#include "../include/Chain-Of-Responsibility.h"

TEST_SUITE("Builder Pattern Tests") {

    TEST_CASE("WorkflowDesigner - Assembly via Director") {
        WorkflowDefinitionBuilder* builder = new StandardDefinitionBuilder();
        WorkflowDesigner designer(builder);

        std::vector<WorkItemSpec> specs = {
            {"UserTask", "ReviewApplication", "Role, Manager"},
            {"ServiceTask", "SendConfirmationEmail", "Endpoint, NotificationAPI"}
        };

        // Use nullptr while Strategy .cpp is not yet linked
        AssignmentStrategy* strategy = new RoleMatchingStrategy("Manager");
        EscalationHandler* handler = new FrontlineHandler(nullptr, 2); 

        //Director contructs the product
        WorkflowDefinition* def = designer.construct("ApprovalPinpeline", specs, strategy, handler);

        REQUIRE(def != nullptr);
        CHECK(def->getName() == "ApprovalPinpeline");
        CHECK(def->getWorkItemSpecs().size() == 2);
        CHECK(def->getWorkItemSpecs()[0].name == "ReviewApplication");
        CHECK(def->getWorkItemSpecs()[1].name == "SendConfirmationEmail");
        CHECK(def->getAssignmentStrategy() == strategy);
        CHECK(def->getEscalationHandler() == handler);

        //safely deletes strategy and handler internally
        delete def; 
    }

    TEST_CASE("StandardDefinitionBuilder - Empty Workflow Throws Exception") {
        WorkflowDefinitionBuilder* builder = new StandardDefinitionBuilder();
        builder->setName("Empty Plan");

        CHECK_THROWS_AS(builder->build(), std::runtime_error);
        delete builder;
    }

    TEST_CASE("StandardDefinitionBuilder - Direct MAnual Construction") {
        StandardDefinitionBuilder* builder = new StandardDefinitionBuilder();
        builder->setName("Manual Pipeline");

        WorkItemSpec spec1 = {"ApprovalTask", "ManagerSignoff", ""};
        builder->addWorkItem(spec1);

        AssignmentStrategy* strategy = new LeastLoadedStrategy();
        builder->setAssignmentRule(strategy);

        WorkflowDefinition* def = builder->build();

        REQUIRE(def != nullptr);
        CHECK(def->getName() == "Manual Pipeline");
        CHECK(def->getWorkItemSpecs().size() == 1);
        CHECK(def->getAssignmentStrategy() == strategy);

        delete def;
        delete builder;
    }

    TEST_CASE("WorkflowDesigner - Construction throws when NULL Builder passed") {
        WorkflowDesigner unattachedDesigner(nullptr);

        std::vector<WorkItemSpec> specs = {
            {"UserTask", "Task1", ""}
        };

        CHECK_THROWS_AS(unattachedDesigner.construct("Test", specs, nullptr, nullptr), std::runtime_error);
    }

}
