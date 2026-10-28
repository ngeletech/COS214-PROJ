/**
 * @file test_template_method.cpp
 * @brief Unit tests for Template Method concrete task classes.
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../include/TemplateMethod.h"

TEST_SUITE("Template Method Pattern Tests") {

    TEST_CASE("UserTask - Initialization and Execution Guard") {
        UserTask userTask("PrepMealStep");
        userTask.setStatus(WorkItemStatus::AVAILABLE);

        CHECK(userTask.getName() == "PrepMealStep");
        CHECK_FALSE(userTask.isComplete());
    }

    TEST_CASE("ServiceTask - Automated Task Setup") {
        ServiceTask serviceTask("SendNotificationAPI");
        serviceTask.setStatus(WorkItemStatus::AVAILABLE);

        CHECK(serviceTask.getName() == "SendNotificationAPI");
        CHECK_FALSE(serviceTask.isComplete());
    }

    TEST_CASE("ApprovalTask - Decision Recording") {
        ApprovalTask approvalTask("ManagerSignoff");
        
        approvalTask.recordDecision(true);
        approvalTask.setStatus(WorkItemStatus::AVAILABLE);

        CHECK(approvalTask.getName() == "ManagerSignoff");
    }

    TEST_CASE("ScriptTask - Key-Value Initialization and Execution") {
        ScriptTask scriptTask("UpdateOrderStatus", "OrderStatus", "IN_PROGRESS");
        scriptTask.setStatus(WorkItemStatus::AVAILABLE);

        CHECK(scriptTask.getName() == "UpdateOrderStatus");
    }
}