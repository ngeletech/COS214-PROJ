/**
 * @file test_template_method.cpp
 * @brief Unit tests for Template Method concrete task classes.
 */

#include "doctest.h"
#include "../include/TemplateMethod.h"
#include "../include/Participant.h"

TEST_SUITE("Template Method Pattern Tests") {

    TEST_CASE("UserTask - Initialization, Execution and State Progression") {
        UserTask userTask("PrepMealStep");
        Participant dummyUser(1, "Chef", "Kitchen");

        CHECK(userTask.getName() == "PrepMealStep");
        CHECK_FALSE(userTask.isComplete());

        userTask.assignTo(&dummyUser);
        userTask.setStatus(WorkItemStatus::ASSIGNED);

        userTask.execute();

        CHECK(userTask.isComplete());
        CHECK(userTask.getStatus() == WorkItemStatus::COMPLETED);

    }

    TEST_CASE("ServiceTask - Automated Task Execution") {
        ServiceTask serviceTask("SendNotificationAPI");
        Participant dummyService(2, "Notifier", "System");  

        CHECK(serviceTask.getName() == "SendNotificationAPI");
        CHECK_FALSE(serviceTask.isComplete());

        serviceTask.assignTo(&dummyService);
        serviceTask.setStatus(WorkItemStatus::ASSIGNED);

        serviceTask.execute();

        CHECK(serviceTask.isComplete());
        CHECK(serviceTask.getStatus() == WorkItemStatus::COMPLETED);

    }

    TEST_CASE("ApprovalTask - Decision Recording and Workflow Progression") {
        ApprovalTask approvalTask("ManagerSignoff");
        Participant dummyManager(3, "Alice", "Manager");
        
        approvalTask.recordDecision(true);

        CHECK(approvalTask.getName() == "ManagerSignoff");
        CHECK_FALSE(approvalTask.isComplete());

        approvalTask.assignTo(&dummyManager);
        approvalTask.setStatus(WorkItemStatus::ASSIGNED);

        approvalTask.execute();

        CHECK(approvalTask.isComplete());
        CHECK(approvalTask.getStatus() == WorkItemStatus::COMPLETED);
    }

    TEST_CASE("ScriptTask - Key-Value Initialization and Execution") {
        ScriptTask scriptTask("UpdateOrderStatus", "OrderStatus", "IN_PROGRESS");
        Participant dummyBot(4, "ScriptRunner", "Bot");

        CHECK(scriptTask.getName() == "UpdateOrderStatus");
        CHECK_FALSE(scriptTask.isComplete());

        scriptTask.assignTo(&dummyBot);
        scriptTask.setStatus(WorkItemStatus::ASSIGNED);

        scriptTask.execute();

        CHECK(scriptTask.isComplete());
        CHECK(scriptTask.getStatus() == WorkItemStatus::COMPLETED);
    }
}