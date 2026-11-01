/**
 * @file scenarios.cpp
 * @brief Pre-configured scenario setups and interactive demo routines.
 */

#include "../include/scenarios.h"
#include "../include/AssignmentStrategy.h"
#include "../include/Chain-Of-Responsibility.h"
#include "../include/Participant.h"
#include "../include/Observer.h"
#include <iostream>
#include <vector>

void setupFoodCateringScenario(TividyFacade& facade) {
    // 1. Register Custom Observers (Observer Pattern)
    std::vector<std::string> recipients = {"Customer (+27821234567)", "KitchenManager"};
    facade.addListener(new NotificationListener("TASK_COMPLETED", recipients, "Order status update: Step finished!"));
    facade.addListener(new DashboardListener());

    // 2. Register Participants
    Participant* chef = new Participant(1, "Chef_John", "Kitchen");
    Participant* supervisor = new Participant(2, "Supervisor_Alice", "Management");
    Participant* manager = new Participant(3, "Branch_Manager", "Executive");
    Participant* courier = new Participant(4, "Courier_Agent", "Delivery");

    facade.registerParticipant(chef);
    facade.registerParticipant(supervisor);
    facade.registerParticipant(manager);
    facade.registerParticipant(courier);

    // 3. Build Escalation Chain with Assigned Participants (Chain of Responsibility)
    EscalationHandler* chain = new FrontlineHandler(chef, 2);
    SupervisorHandler* supHandler = new SupervisorHandler(supervisor, 5);
    ManagerHandler* mgrHandler = new ManagerHandler(manager, 10);
    chain->setNext(supHandler);
    supHandler->setNext(mgrHandler);

    // 4. Define Assignment Strategy (Strategy Pattern)
    AssignmentStrategy* strategy = new RoleMatchingStrategy("Kitchen");

    // 5. Define WorkItem Specifications (Composite + Factory Method)
    std::vector<WorkItemSpec> specs = {
        {"UserTask", "PrepMealStep", ""},
        {"ScriptTask", "UpdateOrderStatus", "OrderStatus, IN_PROGRESS"},
        {"ApprovalTask", "ManagerSignoff", ""},
        {"ServiceTask", "CourierDeliveryAPI", ""}
    };

    // 6. Register Workflow Definition via Facade (Builder Pattern)
    facade.defineWorkflow("MealOrderDelivery", specs, strategy, chain);
    std::cout << "[Setup]: Industry 1 (Food Catering) workflow & Observers registered!\n";
}

void setupTelecomScenario(TividyFacade& facade) {
    // 1. Register Custom Observers (Observer Pattern)
    std::vector<std::string> recipients = {"Client (+27839876543)", "NetworkOps"};
    facade.addListener(new NotificationListener("TASK_COMPLETED", recipients, "Network Update: Step completed successfully!"));

    // 2. Register Participants
    Participant* tech = new Participant(10, "Tech_Bob", "FieldTech");
    Participant* agent = new Participant(11, "Agent_Sarah", "CreditOfficer");
    Participant* eng = new Participant(12, "Eng_David", "NetworkEng");

    tech->setAttribute("department", "Engineering");
    eng->setAttribute("department", "Engineering");

    facade.registerParticipant(tech);
    facade.registerParticipant(agent);
    facade.registerParticipant(eng);

    // 3. Build Escalation Chain (Chain of Responsibility)
    EscalationHandler* chain = new FrontlineHandler(tech, 2);
    chain->setNext(new SupervisorHandler(eng, 5));

    // 4. Define Assignment Strategy (Strategy Pattern)
    AssignmentStrategy* strategy = new AttributeMatchingStrategy("department", "Engineering");

    // 5. Define WorkItem Specifications
    std::vector<WorkItemSpec> specs = {
        {"ApprovalTask", "CreditCheck", ""},
        {"UserTask", "InstallFiberLine", ""},
        {"ScriptTask", "ConfigureRouter", "LineStatus, ACTIVE"},
        {"ServiceTask", "ActivateAccountAPI", ""}
    };

    // 6. Register Workflow Definition via Facade
    facade.defineWorkflow("FiberProvisioning", specs, strategy, chain);
    std::cout << "[Setup]: Industry 2 (Telecom Provisioning) workflow & Observers registered!\n";
}

void runFoodCateringDemo(TividyFacade& facade) {
    std::cout << "\n=======================================================\n";
    std::cout << "   DEMO: Industry 1 - Meal Plan Order-to-Delivery\n";
    std::cout << "=======================================================\n";

    try {
        // --- Added: listDefinitions() ---
        std::cout << "Registered Workflows: ";
        for (const auto& defName : facade.listDefinitions()) {
            std::cout << defName << " ";
        }
        std::cout << "\n\n";

        // 1. Start Workflow Instance
        WorkData initialData;
        initialData.addData("CustomerRegion", "Gauteng");
        initialData.addData("CouponCode", "SAVE20");

        int id = facade.startWorkflow("MealOrderDelivery", initialData);
        std::cout << "-> Order Instance Started [ID: " << id << "]\n";
        std::cout << "-> Initial State: " << facade.trackStatus(id) << "\n";

        // 2. Direct Reassignment (using Participant* directly)
        std::vector<std::string> work = facade.getAvailableWork(id);
        if (!work.empty()) {
            Participant chef(1, "Chef_John", "Kitchen");
            facade.reassignItem(id, work[0], &chef); // --- Added: reassignItem() ---
        }

        // 3. Mutate Data & Test Command Undo
        std::cout << "\n[Step 1]: Updating delivery address data...\n";
        facade.changeData(id, "DeliveryAddress", "123 Campus Way");

        std::cout << "[Step 2]: Executing facade.undoLast() to revert address change...\n";
        facade.undoLast();
        std::cout << "-> Address change undone successfully.\n";

        // 4. Process Available Work Items
        std::cout << "\n[Step 3]: Fetching available work items...\n";
        work = facade.getAvailableWork(id);
        for (const auto& item : work) {
            std::cout << "   * Available Item: " << item << "\n";
        }

        if (!work.empty()) {
            std::cout << "\n[Step 4]: Executing and completing item '" << work[0] << "'...\n";
            facade.completeItem(id, work[0]);
        }

        // 5. Handle Manager Approval Step
        work = facade.getAvailableWork(id);
        for (const auto& item : work) {
            if (facade.isApprovalItem(id, item)) {
                std::cout << "\n[Step 5]: Recording approval decision for '" << item << "'...\n";
                facade.decideItem(id, item, true);
                break;
            }
        }

        // 6. Simulate Kitchen Shortage (Issue Reporting & Chain of Responsibility)
        std::cout << "\n[Step 6]: Simulating ingredient shortage (Reporting Issue)...\n";
        facade.reportIssue(id, "PrepMealStep", "Out of Stock", "Fresh Salmon unavailable", 4);

        // 7. Generate Comprehensive Instance Audit Report
        std::cout << "\n=======================================================\n";
        std::cout << "               FINAL INSTANCE AUDIT REPORT\n";
        std::cout << "=======================================================\n";
        std::cout << facade.generateReport(id) << "\n";

    } catch (const std::exception& e) {
        std::cout << "[Error during Food Catering Demo]: " << e.what() << "\n";
    }
}

void runTelecomDemo(TividyFacade& facade) {
    std::cout << "\n=======================================================\n";
    std::cout << "   DEMO: Industry 2 - Telecom Network Provisioning\n";
    std::cout << "=======================================================\n";

    try {
        WorkData initialData;
        initialData.addData("GPSLocation", "Zone-B");
        initialData.addData("PackageType", "Fiber_100Mbps");

        int id = facade.startWorkflow("FiberProvisioning", initialData);
        std::cout << "-> Provisioning Instance Started [ID: " << id << "]\n";

        // 1. Participant Reassignment Demonstration
        std::vector<TividyFacade::ParticipantInfo> people = facade.listParticipants();
        if (!people.empty()) {
            std::vector<std::string> work = facade.getAvailableWork(id);
            if (!work.empty()) {
                std::cout << "\n[Step 1]: Reassigning item '" << work[0] 
                          << "' to participant '" << people[0].name << "' [ID: " << people[0].id << "]...\n";
                facade.reassignItemToParticipant(id, work[0], people[0].id);
            }
        }

        // 2. Reject Approval Step to Test Rejection & Failure Path
        std::vector<std::string> work = facade.getAvailableWork(id);
        for (const auto& item : work) {
            if (facade.isApprovalItem(id, item)) {
                std::cout << "\n[Step 2]: Rejecting credit approval for '" << item << "'...\n";
                facade.decideItem(id, item, false); // Triggers issue handling & rejection path
                break;
            }
        }

        // 3. Test Workflow Cancellation
        std::cout << "\n[Step 3]: Cancelling provisioning workflow instance...\n";
        facade.cancelWorkflow(id);
        std::cout << "-> Current State: " << facade.trackStatus(id) << "\n";

        // 4. Print Final Audit Report
        std::cout << "\n=======================================================\n";
        std::cout << "               FINAL INSTANCE AUDIT REPORT\n";
        std::cout << "=======================================================\n";
        std::cout << facade.generateReport(id) << "\n";

    } catch (const std::exception& e) {
        std::cout << "[Error during Telecom Demo]: " << e.what() << "\n";
    }
}