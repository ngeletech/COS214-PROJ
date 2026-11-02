#include "TUI.h"
#include "TerminalUI.h"
#include "WorkflowScreens.h"

#include <iostream>
#include <string>
#include <exception>
#include <algorithm>
#include <memory>
#include "AssignmentStrategy.h"

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
        std::vector<std::string> options = {"Start Workflow", "Create Workflow", "Active Workflows", "Exit"};

        int choice = TerminalUI::selectOption("MAIN MENU", options) + 1;

        switch (choice) {
            case 1:
                showStartWorkflow();
                break;
            
            case 2:
                showCreateWorkflow();
                break;

            case 3:
                showActiveWorkflows();
                break;

            case 4:
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
    TerminalUI::showMenuOption(2, "Create Workflow");
    TerminalUI::showMenuOption(3, "Active Workflows");
    TerminalUI::showMenuOption(4, "Exit");

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
        TerminalUI::showInfo("Custom workflow: starting without predefined input fields.");
    }

    try {
        int instanceId = -1;

        TerminalUI::runWithStyledOutput([&]() {
            instanceId = facade.startWorkflow(selectedWorkflow, initialData);
        });

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


/**
 * @brief Builds a new sequential workflow definition through the TUI.
 * The engine owns the strategy after a successful defineWorkflow call.
 */
void TUI::showCreateWorkflow() {
    TerminalUI::clearScreen();
    TerminalUI::showHeader("CREATE WORKFLOW");

    std::string name;

    while (true) {
        std::cout << "Workflow name: ";

        if (!std::getline(std::cin, name)) {
            return;
        }

        if (name.find_first_not_of(" \t\r\n") == std::string::npos) {
            TerminalUI::showError("Workflow name cannot be empty.");
            continue;
        }

        std::vector<std::string> existing = facade.listDefinitions();

        if (std::find(existing.begin(), existing.end(), name) != existing.end()) {
            TerminalUI::showError("A workflow with that name already exists.");
            continue;
        }

        break;
    }

    std::vector<WorkItemSpec> specs;

    const std::vector<std::string> types = {
        "UserTask - manual task",
        "ApprovalTask - approve/reject",
        "ScriptTask - set workflow data",
        "ServiceTask - simulated service",
        "Finish and review",
        "Cancel"
    };

    while (true) {
        TerminalUI::clearScreen();
        TerminalUI::showHeader("DESIGN WORKFLOW | " + name);

        for (std::size_t i = 0; i < specs.size(); ++i) {
            std::cout << (i + 1) << ". " << specs[i].name
                      << " [" << specs[i].type << "]\n";
        }

        std::cout << "\n";

        int selected = TerminalUI::selectOption(
            "ADD A WORK ITEM",
            types,
            false
        );

        if (selected == 5) {
            return;
        }

        if (selected == 4) {
            if (specs.empty()) {
                TerminalUI::showError("Add at least one work item first.");
                TerminalUI::waitForContinue();
                continue;
            }

            break;
        }

        WorkItemSpec spec;

        if (selected == 0) {
            spec.type = "UserTask";
        }
        else if (selected == 1) {
            spec.type = "ApprovalTask";
        }
        else if (selected == 2) {
            spec.type = "ScriptTask";
        }
        else {
            spec.type = "ServiceTask";
        }

        TerminalUI::clearScreen();
        TerminalUI::showHeader("CONFIGURE " + spec.type);

        while (true) {
            std::cout << "Work item name: ";

            if (!std::getline(std::cin, spec.name)) {
                return;
            }

            if (spec.name.find_first_not_of(" \t\r\n") == std::string::npos) {
                TerminalUI::showError("Work item name cannot be empty.");
                continue;
            }

            bool duplicate = false;

            for (std::size_t i = 0; i < specs.size(); ++i) {
                if (specs[i].name == spec.name) {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate) {
                TerminalUI::showError("Work item names must be unique.");
                continue;
            }

            break;
        }

        if (spec.type == "ScriptTask") {
            std::string key;
            std::string value;

            while (true) {
                std::cout << "Data key: ";

                if (!std::getline(std::cin, key)) {
                    return;
                }

                if (key.find_first_not_of(" \t\r\n") != std::string::npos &&
                    key.find(',') == std::string::npos) {
                    break;
                }

                TerminalUI::showError(
                    "Use a non-empty key without commas."
                );
            }

            std::cout << "Data value: ";

            if (!std::getline(std::cin, value)) {
                return;
            }

            spec.config = key + "," + value;
        }

        specs.push_back(spec);

        TerminalUI::showSuccess(
            "Added " + spec.name + " (" + spec.type + ")."
        );

        TerminalUI::waitForContinue();
    }

    TerminalUI::clearScreen();
    TerminalUI::showHeader("REVIEW WORKFLOW | " + name);

    for (std::size_t i = 0; i < specs.size(); ++i) {
        std::cout << (i + 1) << ". " << specs[i].name << " (" << specs[i].type << ")";

        if (!specs[i].config.empty()) {
            std::cout << " | " << specs[i].config;
        }

        std::cout << "\n";
    }

    std::cout << "\nTasks will execute sequentially. Existing participants are used.\n" << "Custom definitions last only until the application exits.\n\n";

    const std::vector<std::string> confirm = {"Create Workflow", "Cancel"};

    if (TerminalUI::selectOption("CONFIRM", confirm, false) != 0) {
        return;
    }

    try {
        std::unique_ptr<AssignmentStrategy> strategy(new LeastLoadedStrategy());

        facade.defineWorkflow(name, specs, strategy.get(), nullptr);

        strategy.release();

        TerminalUI::showSuccess("Created '" + name + "'. Find it under Start Workflow.");
    }
    catch (const std::exception& error) {
        TerminalUI::showError(error.what());
    }
    TerminalUI::waitForContinue();
}
