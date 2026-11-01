#include "../include/Scenarios.h"
#include "../include/AssignmentStrategy.h"
#include "../include/Chain-Of-Responsibility.h"
#include "../include/Participant.h"
#include <iostream>

void setupFoodCateringScenario(TividyFacade& facade) {
    //Register Participants
    facade.registerParticipant(new Participant(1, "Chef John", "Kitchen"));
    facade.registerParticipant(new Participant(2, "Supervisor: Alice", "Management"));
    facade.registerParticipant(new Participant(3, "Courier Agent", "Delivery"));

    //Build Escalation Chain
    //KitchenStaff -> Supervisor -> BranchManager
    EscalationHandler* chain = new FrontlineHandler(nullptr, 2);
    SupervisorHandler* supHandler = new SupervisorHandler(nullptr, 5);
    ManagerHandler* mgrHandler = new ManagerHandler(nullptr, 10);
    chain->setNext(supHandler);
    supHandler->setNext(mgrHandler);

    //Define Strategy
    RoleMatchingStrategy* strategy = new RoleMatchingStrategy("Kitchen");
    strategy->requireRoleFor("ManagerSignoff", "Management");

    //Define WorkItem Specifications for Meal Order
    std::vector<WorkItemSpec> specs = {
        {"UserTask", "PrepMealStep", ""},
        {"ScriptTask", "UpdateOrderStatus", "OrderStatus, IN_PROGRESS"},
        {"ApprovalTask", "ManagerSignoff", ""},
        {"ServiceTask", "CourierDeliveryAPI", ""}
    };

    //Register Workflow Definition with Facade
    facade.defineWorkflow("MealOrderDelivery", specs, strategy, chain);
    std::cout << "[Setup]: Industry 1 (Food Catering) workflow successfully registered!" << std::endl;
}

void setupTelecomScenario(TividyFacade& facade) {
    //Register Participants
    facade.registerParticipant(new Participant(10, "Techician Bob", "FieldTech"));
    facade.registerParticipant(new Participant(11, "Agent Sarah", "CreditOfficer"));
    //facade.registerParticipant(new Participant(12, "Eng David", "NetworkEng"));
    Participant* david = new Participant(12, "Eng David", "NetworkEng");
    david->setAttribute("department", "Engineering");
    facade.registerParticipant(david);

    //Build Escalation Chain
    EscalationHandler* chain = new FrontlineHandler(nullptr, 2);
    chain->setNext(new SupervisorHandler(nullptr, 5));

    //Define Strategy
    AssignmentStrategy* strategy = new RoleMatchingStrategy("CreditOfficer");

    //Define WorkItem Specifications for Fiber Provisioning
    std::vector<WorkItemSpec> specs = {
        {"ApprovalTask", "CreditCheck", ""},
        {"UserTask", "ScheduleInstallation", ""},
        {"UserTask", "InstallFiberLine", ""},
        {"ScriptTask", "ConfigureRouter", "LineStatus, ACTIVE"},
        {"ServiceTask", "ActivateAccountAPI", ""}
    };

    //Register Workflow Definition with Facade
    facade.defineWorkflow("FiberProvisioning", specs, strategy, chain);
    std::cout << "[Setup]: Industry 2 (Telecom Provisioning) workflow successfully registered!" << std::endl;
}

void runFoodCateringDemo(TividyFacade& facade) {
    std::cout << "\nRunning Industry 1: Food Catering Demo " << std::endl;
    WorkData initialData;
    initialData.addData("CustomerRegion", "Gauteng");
    initialData.addData("CouponCode", "SAVE20");

    int instanceId = facade.startWorkflow("MealOrderDelivery", initialData);
    std::cout << "Order Instance Started. ID: " << instanceId << std::endl;
    std::cout << "Current Status: " << facade.trackStatus(instanceId) << std::endl;
}

void runTelecomDemo(TividyFacade& facade) {
    std::cout << "\nRunning Industry 2: Telecom Provisioning Demo " << std::endl;
    WorkData initialData;
    initialData.addData("GPSLocation", "Zone-B");
    initialData.addData("PackageType", "Fiber_100Mbps");

    int instanceId = facade.startWorkflow("FiberProvisioning", initialData);
    std::cout << "Provisioning Instance Started. ID: " << instanceId << std::endl;
    std::cout << "Current Status: " << facade.trackStatus(instanceId) << std::endl;
}