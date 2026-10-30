/**
 * @file test_template-method.cpp
 * @brief Unit tests for Template Method concrete task classes.
 */

#include "doctest.h"
#include "../include/TemplateMethod.h"
#include "../include/Participant.h"

TEST_SUITE("Template Method Pattern Tests") {

    TEST_CASE("UserTask - Execution and Manual Completion") {
        UserTask userTask("PrepMealStep");
        Participant dummyUser(1, "Chef", "Kitchen");

        CHECK(userTask.getName() == "PrepMealStep");
        CHECK_FALSE(userTask.isComplete());

        userTask.assignTo(&dummyUser);
        userTask.setStatus(WorkItemStatus::ASSIGNED);

        userTask.execute();
        CHECK(userTask.getStatus() == WorkItemStatus::STARTED);

        userTask.complete();
        CHECK(userTask.isComplete());
        CHECK(userTask.getStatus() == WorkItemStatus::COMPLETED);
    }

    TEST_CASE("ServiceTask - Automated Execution and Auto-Completion") {
        ServiceTask serviceTask("SendNotificationAPI");

        CHECK(serviceTask.getName() == "SendNotificationAPI");
        CHECK_FALSE(serviceTask.isComplete());

        serviceTask.setStatus(WorkItemStatus::AVAILABLE);

        // execute() runs doWork() and auto-completes because requiresAssignee() == false
        serviceTask.execute();

        CHECK(serviceTask.isComplete());
        CHECK(serviceTask.getStatus() == WorkItemStatus::COMPLETED);
    }

    TEST_CASE("ApprovalTask - Decision Recording and Manual Completion") {
        ApprovalTask approvalTask("ManagerSignoff");
        Participant dummyManager(3, "Alice", "Manager");
        
        approvalTask.recordDecision(true);

        CHECK(approvalTask.getName() == "ManagerSignoff");
        CHECK_FALSE(approvalTask.isComplete());

        approvalTask.assignTo(&dummyManager);
        approvalTask.setStatus(WorkItemStatus::ASSIGNED);

        approvalTask.execute();
        CHECK(approvalTask.getStatus() == WorkItemStatus::STARTED);

        approvalTask.complete();
        CHECK(approvalTask.isComplete());
        CHECK(approvalTask.getStatus() == WorkItemStatus::COMPLETED);
    }

    TEST_CASE("ScriptTask - Key-Value Execution and Auto-Completion") {
        ScriptTask scriptTask("UpdateOrderStatus", "OrderStatus", "IN_PROGRESS");

        CHECK(scriptTask.getName() == "UpdateOrderStatus");
        CHECK_FALSE(scriptTask.isComplete());

        scriptTask.setStatus(WorkItemStatus::AVAILABLE);

        scriptTask.execute();

        CHECK(scriptTask.isComplete());
        CHECK(scriptTask.getStatus() == WorkItemStatus::COMPLETED);
    }
}