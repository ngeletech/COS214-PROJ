#include "TUI.h"
#include "TerminalUI.h"
#include "WorkflowScreens.h"

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
        std::vector<std::string> options = {"Start Workflow", "Active Workflows", "Exit"};

        int choice = TerminalUI::selectOption("MAIN MENU", options) + 1;

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

    std::vector<std::string> options = definitions;
    options.push_back("Back to Main Menu");

    int choice = TerminalUI::selectOption("SELECT A WORKFLOW", options);

    if (choice == static_cast<int>(definitions.size())) {
        return;
    }

    const std::string& selectedWorkflow = definitions[choice];

    WorkData initialData;

    std::cout << "\n";

    // Collect the initial data required by the selected industry workflow.
    if (selectedWorkflow == "MealOrderDelivery") {
        std::string region;
        std::string coupon;

        while (true) {
            std::cout << "Enter customer region: ";
            std::getline(std::cin, region);

            if (!region.empty() &&
                region.find_first_not_of(" \t\n\r") != std::string::npos) {
                break;
            }

            TerminalUI::showError("Customer region cannot be empty.");
        }

        std::cout << "Enter coupon code " << "\033[38;2;160;165;181m" << "(optional)" << "\033[0m" << ": ";
        std::getline(std::cin, coupon);

        initialData.addData("CustomerRegion", region);
        initialData.addData("CouponCode", coupon);
    }
    else if (selectedWorkflow == "FiberProvisioning") {
        std::string location;
        std::string package;

        while (true) {
            std::cout << "Enter GPS location: ";
            std::getline(std::cin, location);

            if (!location.empty() &&
                location.find_first_not_of(" \t\n\r") != std::string::npos) {
                break;
            }

            TerminalUI::showError("GPS location cannot be empty.");
        }

        std::vector<std::string> packages = {"Basic - 20 Mbps", "Standard - 50 Mbps", "Premium - 100 Mbps", "Ultra - 200 Mbps"};

        int choice = TerminalUI::selectOption(
            "Select a fiber package:",
            packages
        );

        package = packages[choice];

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

    std::vector<std::string> options;

    for (std::size_t i = 0; i < activeInstances.size(); ++i) {
        int instanceId = activeInstances[i];

        try {
            options.push_back("Instance " + std::to_string(instanceId)+ " | " + facade.trackStatus(instanceId));
        }
        catch (const std::exception& error) {
            options.push_back("Instance " + std::to_string(instanceId)+ " | Status unavailable");
        }
    }

    options.push_back("Back to Main Menu");

    int choice = TerminalUI::selectOption("ACTIVE WORKFLOWS", options);

    if (choice == static_cast<int>(activeInstances.size())) {
        return;
    }
    showWorkflowOverview(activeInstances[choice]);
}

/**
 * @brief Displays the overview and current report for a workflow instance.
 * @param instanceId ID of the selected workflow instance.
 */

void TUI::showWorkflowOverview(int instanceId) {
    WorkflowScreens screens(facade);
    screens.showOverview(instanceId);
}