#include "WorkflowScreens.h"
#include "TerminalUI.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <exception>
#include <limits>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <functional>
#include <stdexcept>

namespace {
    const std::string RESET = "\033[0m", TEAL = "\033[38;2;54;199;176m";
    const std::string WHITE = "\033[38;2;241;242;246m", GREY = "\033[38;2;160;165;181m";
    const std::string BLUE = "\033[38;2;112;167;245m", AMBER = "\033[38;2;242;183;91m";
    const std::string GREEN = "\033[38;2;66;201;138m", RED = "\033[38;2;239;114;122m";

    struct ProgressItem {
        std::string name;
        std::string status;
        std::string assignee;
    };

    // Convert internal task names into audience-friendly names.
    std::string friendlyTaskName(const std::string& name) {

        if (name == "PrepMealStep") return "Meal Preparation";
        if (name == "UpdateOrderStatus") return "Update Order Status";
        if (name == "ManagerSignoff") return "Manager Approval";
        if (name == "CourierDeliveryAPI") return "Courier Delivery";

        if (name == "CreditCheck") return "Credit Check";
        if (name == "InstallFiberLine") return "Install Fibre Line";
        if (name == "ConfigureRouter") return "Configure Router";
        if (name == "ActivateAccountAPI") return "Activate Customer Account";

        return name;
    }

    std::vector<ProgressItem> getProgressItems(const std::string& report) {
        std::vector<ProgressItem> items;
        std::istringstream stream(report);
        std::string line;
        bool readingWorkItems = false;

        while (std::getline(stream, line)) {
            // Remove leading whitespace from report lines.
            std::size_t first = line.find_first_not_of(" \t\r\n");

            if (first == std::string::npos) {
                continue;
            }

            line = line.substr(first);

            // Remove trailing whitespace, including Windows line endings.
            std::size_t last = line.find_last_not_of(" \t\r\n");

            if (last != std::string::npos) {
                line = line.substr(0, last + 1);
            }

            if (line == "Work items:") {
                readingWorkItems = true;
                continue;
            }

            if (line == "History:") {
                break;
            }

            if (!readingWorkItems || line.empty()) {
                continue;
            }

            std::size_t colon = line.find(": ");
            if (colon == std::string::npos) {
                continue;
            }

            ProgressItem item;
            item.name = line.substr(0, colon);

            std::string details = line.substr(colon + 2);

            std::size_t openBracket = details.find(" (");
            if (openBracket != std::string::npos) {
                item.status = details.substr(0, openBracket);

                std::size_t closeBracket = details.find(')', openBracket);
                if (closeBracket != std::string::npos) {
                    item.assignee = details.substr(
                        openBracket + 2,
                        closeBracket - openBracket - 2
                    );
                }
            } else {
                item.status = details;
                item.assignee = "Unassigned";
            }

            items.push_back(item);
        }

        return items;
    }

    std::string statusColor(const std::string& status) {
        if (status == "Running" || status == "Completed" || status == "completed") {
            return GREEN;
        }
        if (status == "started" || status == "assigned") {
            return BLUE;
        }
        if (status == "available") {
            return AMBER;
        }
        if (status == "Failed" || status == "Cancelled" || status == "cancelled" || status == "rejected") {
            return RED;
        }
        return GREY;
    }

    void displayProgressTracker(const std::string& report) {
    std::vector<ProgressItem> items = getProgressItems(report);

    std::cout << "\n" << TEAL << "\033[1m";
    std::cout << "WORKFLOW PROGRESS";
    std::cout << RESET << "\n";

    if (items.empty()) {
        TerminalUI::showInfo("No workflow tasks found.");
        return;
    }

    int completed = 0;
    std::string currentTask = "No active task";
    std::string currentAssignee = "Unassigned";

    for (std::size_t i = 0; i < items.size(); ++i) {
        if (items[i].status == "completed") {
            ++completed;
        }

        if (currentTask == "No active task" &&
            (items[i].status == "started" ||
            items[i].status == "assigned" ||
            items[i].status == "available")) {
            currentTask = friendlyTaskName(items[i].name);
            currentAssignee = items[i].assignee.empty() ? "Unassigned" : items[i].assignee;
        }
    }

    std::cout << BLUE << "Current task: " << WHITE
              << currentTask << RESET << "\n";

    std::cout << BLUE << "Current assignee: " << WHITE
              << currentAssignee << RESET << "\n";

    std::cout << GREY << "Progress: " << RESET
              << GREEN << completed << RESET
              << " / " << items.size()
              << " tasks completed\n\n";

    for (std::size_t i = 0; i < items.size(); ++i) {
        const ProgressItem& item = items[i];

        std::string marker = "[ ]";
        std::string displayStatus = item.status;

        if (item.status == "completed") {
            marker = "[✓]";
            displayStatus = "Completed";
        } else if (item.status == "started") {
            marker = "[>]";
            displayStatus = "In Progress";
        } else if (item.status == "assigned") {
            marker = "[A]";
            displayStatus = "Assigned";
        } else if (item.status == "available") {
            marker = "[ ]";
            displayStatus = "Ready";
        } else if (item.status == "created") {
            marker = "[~]";
            displayStatus = "Created";
        } else if (item.status == "rejected") {
            marker = "[!]";
            displayStatus = "Rejected";
        } else if (item.status == "cancelled") {
            marker = "[!]";
            displayStatus = "Cancelled";
        }

        std::cout << statusColor(item.status) << marker << RESET
                  << " " << WHITE << friendlyTaskName(item.name) << RESET
                  << "  " << statusColor(item.status)
                  << displayStatus << RESET << "\n";

        std::cout << GREY << "    Assignee: "
                  << (item.assignee.empty() ? "Unassigned" : item.assignee)
                  << RESET << "\n";
    }

    std::cout << "\n";
}

    std::string promptText(const std::string& prompt, bool required) {
        while (true) {
            std::cout << TEAL << prompt << RESET << std::flush;
            std::string value;

            if (!std::getline(std::cin, value)) {
                throw std::runtime_error("Input closed.");
            }
            if (!required || value.find_first_not_of(" \t\r\n") != std::string::npos) {
                return value;
            }
            TerminalUI::showError("This field cannot be empty.");
        }
    }

    int promptSeverity() {
        while (true) {
            std::string raw = promptText("Severity (1-10): ", true);
            char* end = nullptr;
            long n = std::strtol(raw.c_str(), &end, 10);

            if (end != raw.c_str() && *end == '\0' && n >= 1 && n <= 10) {
                return static_cast<int>(n);
            }
            TerminalUI::showError("Enter a whole number from 1 to 10.");
        }
    }

    void showResult(const std::string& message) {
        TerminalUI::showSuccess(message);
        TerminalUI::waitForContinue();
    }
    void displayEscalationResult(
        const std::string& taskName,
        const std::string& issueType,
        const std::string& description,
        int severity
    ) {
        TerminalUI::clearScreen();
        TerminalUI::showHeader("ESCALATION RESULT");

        std::cout << "\n";
        std::cout << TEAL << "ISSUE DETAILS\n" << RESET;

        std::cout << WHITE << "Work item: " << RESET
                << friendlyTaskName(taskName) << "\n";

        std::cout << WHITE << "Issue: " << RESET
                << issueType << "\n";

        std::cout << WHITE << "Description: " << RESET
                << description << "\n";

        std::cout << WHITE << "Severity: " << RESET
                << severity << " / 10\n";

        std::cout << "\n";
        std::cout << TEAL << "ESCALATION STATUS\n" << RESET;
        std::cout << GREEN << "[✓] Issue submitted successfully\n" << RESET;

        std::cout << WHITE << "Outcome: " << RESET
                << AMBER << "Submitted for escalation" << RESET << "\n";

        std::cout << GREY
                << "\nThe issue has been forwarded to the escalation process."
                << RESET << "\n";

        TerminalUI::waitForContinue();
    }
    std::string getMessageColor(const std::string& label) {
        if (label == "[Task]") {
            return BLUE;
        }
        if (label == "[ScriptTask]") {
            return TEAL;
        }
        if (label == "[ApprovalTask]") {
            return AMBER;
        }
        if (label == "[ServiceTask]") {
            return GREEN;
        }
        if (label == "[SUCCESS]") {
            return GREEN;
        }
        if (label == "[ERROR]") {
            return RED;
        }
        return GREY;
    }

    void printEngineMessage(const std::string& message) {
        if (message.empty()) {
            return;
        }

        std::size_t end = message.find(']');

        if (!message.empty() && message[0] == '[' && end != std::string::npos) {
            std::string label = message.substr(0, end + 1);
            std::string description = message.substr(end + 1);

            std::cout << getMessageColor(label) << label << RESET << WHITE << description << RESET << "\n";
        }
        else {
            std::cout << GREY << message << RESET << "\n";
        }

        std::cout.flush();
    }
    void runWithStyledOutput(const std::function<void()>& action) {
        std::ostringstream captured;
        std::streambuf* original = std::cout.rdbuf(captured.rdbuf());

        std::exception_ptr error;

        try {
            action();
        }
        catch (...) {
            error = std::current_exception();
        }

        std::cout.rdbuf(original);

        std::istringstream messages(captured.str());
        std::string line;

        while (std::getline(messages, line)) {
            printEngineMessage(line);

            if (!line.empty()) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(110)
                );
            }
        }

        if (error) {
            std::rethrow_exception(error);
        }
    }
}

WorkflowScreens::WorkflowScreens(TividyFacade& f) : facade(f) {}

void WorkflowScreens::showOverview(int id) {
    while (true) {
        TerminalUI::clearScreen();
        TerminalUI::showHeader("WORKFLOW OVERVIEW | INSTANCE " + std::to_string(id));

        try {
            std::string status = facade.trackStatus(id);
            std::cout << "\n";
            std::cout << TEAL << "WORKFLOW STATUS: " << statusColor(status) << status << RESET << "\n";
            std::cout << GREY << "----------------------------------------" << RESET << "\n";
            std::string report = facade.generateReport(id);
            displayProgressTracker(report);
            std::cout << GREY << "----------------------------------------" << RESET << "\n";
        } catch (const std::exception& e) {
            TerminalUI::showError(e.what());
            TerminalUI::waitForContinue();
            return;
        }

        std::vector<std::string> choices = {
            "View Available Work / Interact",
            "Report Issue",
            "View Execution History",
            "Undo Last Command (global)",
            "Refresh Overview",
            "Back"
        };
        int choice = TerminalUI::selectOption("ACTIONS", choices, false);

        if (choice == 5) {
            return;
        }
        if (choice == 4) {
            continue;
        }
        if (choice == 0) {
            showAvailableWork(id);
        }
        if (choice == 1) {
            reportIssue(id);
        }
        if (choice == 2) {
            showHistory(id);
        }
        if (choice == 3) {
            undoLastCommand();
        }
    }
}

void WorkflowScreens::showAvailableWork(int id) {
    while (true) {
        TerminalUI::clearScreen();
        TerminalUI::showHeader("AVAILABLE WORK | INSTANCE " + std::to_string(id));

        try {
            std::vector<std::string> items = facade.getAvailableWork(id);
            if (items.empty()) {
                TerminalUI::showInfo("No available work items.");
                TerminalUI::waitForContinue();
                return;
            }
            std::vector<std::string> choices;
            for (std::size_t i = 0; i < items.size(); ++i) {
                choices.push_back(friendlyTaskName(items[i]));
            }
            choices.push_back("Back");
            int choice = TerminalUI::selectOption("SELECT WORK ITEM", choices, false);
            if (choice == static_cast<int>(items.size())) {
                return;
            }
            // Pass the original task name to the workflow engine.
            interactWithItem(id, items[choice]);
        } catch (const std::exception& e) {
            TerminalUI::showError(e.what());
            TerminalUI::waitForContinue();
            return;
        }
    }
}

void WorkflowScreens::interactWithItem(int id, const std::string& name) {
    while (true) {
        TerminalUI::clearScreen();
        TerminalUI::showHeader("WORK ITEM | " + friendlyTaskName(name));
        bool approval = false;

        try {
            approval = facade.isApprovalItem(id, name);
        } catch (const std::exception& e) {
            TerminalUI::showError(e.what());
            TerminalUI::waitForContinue();
            return;
        }

        std::vector<std::string> choices;
        if (approval) {
            choices.push_back("Approve");
            choices.push_back("Reject");
        } else {
            choices.push_back("Complete Task");
        }
        choices.push_back("Reassign Task");
        choices.push_back("Report Issue");
        choices.push_back("Back");

        int choice = TerminalUI::selectOption("SELECT ACTION", choices, false);
        if (choice == static_cast<int>(choices.size()) - 1) {
            return;
        }

        try {
            if (approval && (choice == 0 || choice == 1)) {
                runWithStyledOutput([&]() {
                    facade.decideItem(id, name, choice == 0);
                });
                showResult(choice == 0 ? "Approval submitted." : "Rejection submitted to escalation.");
                return;
            }
            if (!approval && choice == 0) {
                runWithStyledOutput([&]() {
                    facade.completeItem(id, name);
                });
                showResult("Task completed.");
                return;
            }


            int reassignIndex = approval ? 2 : 1;
            if (choice == reassignIndex) {
                std::vector<TividyFacade::ParticipantInfo> people = facade.listParticipants();
                if (people.empty()) {
                    TerminalUI::showError("No registered participants.");
                    TerminalUI::waitForContinue();
                    continue;
                }

                std::vector<std::string> labels;
                for (std::size_t i = 0; i < people.size(); ++i) {
                    labels.push_back(people[i].name + " (" + people[i].role + ") | ID " + std::to_string(people[i].id));
                }
                labels.push_back("Back");
                int picked = TerminalUI::selectOption("REASSIGN TO", labels);

                if (picked == static_cast<int>(people.size())) {
                    continue;
                }
                runWithStyledOutput([&]() {
                    facade.reassignItemToParticipant(id, name, people[picked].id);
                });
                showResult("Work item reassigned.");
                return;
            }

            std::string type = promptText("Issue type: ", true);
            std::string desc = promptText("Description: ", true);
            int severity = promptSeverity();
            runWithStyledOutput([&]() {
                facade.reportIssue(id, name, type, desc, severity);
            });

            std::string state = facade.trackStatus(id);

            if (state == "Failed") {
                std::cout << RED << "Workflow Failed" << RESET << "\n";
                std::cout << "The issue could not be resolved or the retry limit was reached.\n";
            }
            else {
                std::cout << GREEN << "Issue Handled" << RESET << "\n";
                std::cout << "Workflow Continued\n";
            }

            TerminalUI::waitForContinue();
            return;
        } catch (const std::exception& e) {
            TerminalUI::showError(e.what());
            TerminalUI::waitForContinue();
        }
    }
}

void WorkflowScreens::reportIssue(int id) {
    try {
        std::vector<std::string> items = facade.getAvailableWork(id);
        if (items.empty()) {
            TerminalUI::showInfo("No available work items to report against.");
            TerminalUI::waitForContinue();
            return;
        }

        std::vector<std::string> choices;
        for (std::size_t i = 0; i < items.size(); ++i) {
            choices.push_back(friendlyTaskName(items[i]));
        }
        choices.push_back("Back");
        int choice = TerminalUI::selectOption("SELECT WORK ITEM FOR ISSUE", choices);
        if (choice == static_cast<int>(items.size()) - 1) {
            return;
        }

        std::string type = promptText("Issue type: ", true);
        std::string description = promptText("Description: ", true);
        int severity = promptSeverity();
        runWithStyledOutput([&]() {
            facade.reportIssue(id, items[choice], type, description, severity);
        });
        std::string state = facade.trackStatus(id);
        if (state == "Failed") {
            std::cout << RED << "Workflow Failed" << RESET << "\n";
            std::cout << "The issue could not be resolved or the retry limit was reached.\n";
        }
        else {
            std::cout << GREEN << "Issue Handled" << RESET << "\n";
            std::cout << "Workflow Continued\n";
        }
        TerminalUI::waitForContinue();
    } catch (const std::exception& e) {
        TerminalUI::showError(e.what());
        TerminalUI::waitForContinue();
    }
}

void WorkflowScreens::showHistory(int id) {
    TerminalUI::clearScreen();
    TerminalUI::showHeader("EXECUTION HISTORY | INSTANCE " + std::to_string(id));

    try {
        std::string report = facade.generateReport(id);
        std::size_t start = report.find("History:");
        if (start == std::string::npos) {
            TerminalUI::showInfo("History section unavailable.");
        } else {
            std::cout << GREY << report.substr(start) << RESET << "\n";
        }
    } catch (const std::exception& e) {
        TerminalUI::showError(e.what());
    }
    TerminalUI::waitForContinue();
}

void WorkflowScreens::undoLastCommand() {
    TerminalUI::clearScreen();
    TerminalUI::showHeader("UNDO LAST COMMAND");
    std::cout << AMBER << "WARNING: Undo affects the latest command across ALL workflow instances.\n" << "Completion, approval and issue reporting are not recorded as undoable commands.\n" << RESET << "\n";

    std::vector<std::string> choices = {"No, go back", "Yes, undo latest command"};
    if (TerminalUI::selectOption("CONFIRM UNDO", choices, false) != 1) {
        return;
    }

    try {
        facade.undoLast();
        TerminalUI::showInfo("Undo requested. Review the workflow report for the resulting state.");
    } catch (const std::exception& e) {
        TerminalUI::showError(e.what());
    }
    TerminalUI::waitForContinue();
}
