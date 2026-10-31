#include "TUI.h"
#include "TerminalUI.h"

#include <iostream>
#include <string>
#include <exception>

 /**
  * @brief Constructs the TUI using the existing workflow Facade.
  * @param facade Reference to the workflow engine Facade.
  */
TUI::TUI(TividyFacade& facade)
    : facade(facade) {
}

/**
 * @brief Starts the interactive terminal application.
 */
void TUI::run() {
    TerminalUI::enableColors();

    bool running = true;

    while (running) {
        TerminalUI::clearScreen();
        showMainMenu();

        int choice = TerminalUI::promptInt("Select an option: ");

        switch (choice) {
            case 1:
                showStartWorkflow();
                break;

            case 2:
                showActiveWorkflows();
                break;

            case 3:
                running = false;
                break;

            default:
                TerminalUI::showError("Invalid menu option.");
                TerminalUI::waitForContinue();
                break;
        }
    }

    TerminalUI::clearScreen();
    TerminalUI::showHeader("Application closed");
    TerminalUI::showSuccess("Thank you for using TIVIDY.");
}

/**
 * @brief Displays the main navigation menu.
 */
void TUI::showMainMenu() {
    TerminalUI::showHeader("MAIN MENU");

    TerminalUI::showMenuOption(1, "Start Workflow");
    TerminalUI::showMenuOption(2, "Active Workflows");
    TerminalUI::showMenuOption(3, "Exit");

    TerminalUI::showDivider();
}

/**
 * @brief Displays available workflow definitions and starts a selected workflow.
 */
void TUI::showStartWorkflow() {
    TerminalUI::clearScreen();
    TerminalUI::showHeader("START A NEW WORKFLOW");

    std::vector<std::string> definitions = facade.listDefinitions();

    if (definitions.empty()) {
        TerminalUI::showError("No workflow definitions are available.");
        TerminalUI::waitForContinue();
        return;
    }

    std::cout << "Available workflows:\n\n";

    for (std::size_t i = 0; i < definitions.size(); ++i) {
        TerminalUI::showMenuOption(static_cast<int>(i + 1), definitions[i]);
    }

    TerminalUI::showMenuOption(0, "Back to Main Menu");
    TerminalUI::showDivider();

    int choice = TerminalUI::promptInt("Select a workflow: ");

    if (choice == 0) {
        return;
    }

    if (choice < 1 || choice > static_cast<int>(definitions.size())) {
        TerminalUI::showError("Invalid workflow selection.");
        TerminalUI::waitForContinue();
        return;
    }

    const std::string& selectedWorkflow = definitions[choice - 1];

    WorkData initialData;

    std::cout << "\n";

    // Collect the initial data required by the selected industry workflow.
    if (selectedWorkflow == "MealOrderDelivery") {
        std::string region;
        std::string coupon;

        std::cout << "Enter customer region: ";
        std::getline(std::cin, region);

        std::cout << "Enter coupon code: ";
        std::getline(std::cin, coupon);

        initialData.addData("CustomerRegion", region);
        initialData.addData("CouponCode", coupon);
    }
    else if (selectedWorkflow == "FiberProvisioning") {
        std::string location;
        std::string package;

        std::cout << "Enter GPS location: ";
        std::getline(std::cin, location);

        std::cout << "Enter package type: ";
        std::getline(std::cin, package);

        initialData.addData("GPSLocation", location);
        initialData.addData("PackageType", package);
    }
    else {
        TerminalUI::showError("No input configuration exists for this workflow.");
        TerminalUI::waitForContinue();
        return;
    }

    try {
        int instanceId = facade.startWorkflow(selectedWorkflow, initialData);

        activeInstances.push_back(instanceId);

        TerminalUI::clearScreen();
        TerminalUI::showHeader("WORKFLOW STARTED");

        TerminalUI::showSuccess(
            "Workflow started successfully. Instance ID: "
            + std::to_string(instanceId)
        );

        showWorkflowOverview(instanceId);
    }
    catch (const std::exception& error) {
        TerminalUI::showError(error.what());
        TerminalUI::waitForContinue();
    }
}

/**
 * @brief Displays workflows started during the current application session.
 */
void TUI::showActiveWorkflows() {
    TerminalUI::clearScreen();
    TerminalUI::showHeader("ACTIVE WORKFLOWS");

    if (activeInstances.empty()) {
        TerminalUI::showInfo("No workflows have been started during this session.");
        TerminalUI::waitForContinue();
        return;
    }

    for (std::size_t i = 0; i < activeInstances.size(); ++i) {
        int instanceId = activeInstances[i];

        try {
            std::cout << "\n";
            TerminalUI::showMenuOption(
                static_cast<int>(i + 1),
                "Instance " + std::to_string(instanceId)
                + " | " + facade.trackStatus(instanceId)
            );
        }
        catch (const std::exception& error) {
            TerminalUI::showError(error.what());
        }
    }

    TerminalUI::showMenuOption(0, "Back to Main Menu");
    TerminalUI::showDivider();

    int choice = TerminalUI::promptInt("Select a workflow: ");

    if (choice == 0) {
        return;
    }

    if (choice < 1 || choice > static_cast<int>(activeInstances.size())) {
        TerminalUI::showError("Invalid workflow selection.");
        TerminalUI::waitForContinue();
        return;
    }

    showWorkflowOverview(activeInstances[choice - 1]);
}

/**
 * @brief Displays the overview and current report for a workflow instance.
 * @param instanceId ID of the selected workflow instance.
 */
void TUI::showWorkflowOverview(int instanceId) {
    TerminalUI::clearScreen();
    TerminalUI::showHeader(
        "WORKFLOW OVERVIEW | INSTANCE "
        + std::to_string(instanceId)
    );

    try {
        std::cout << "Current Status: "
                  << facade.trackStatus(instanceId) << "\n\n";

        TerminalUI::showDivider();

        std::cout << facade.generateReport(instanceId) << "\n";

        TerminalUI::showDivider();
    }
    catch (const std::exception& error) {
        TerminalUI::showError(error.what());
    }

    TerminalUI::waitForContinue();
}