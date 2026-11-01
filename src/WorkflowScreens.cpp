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
#include <exception>

namespace {
    const std::string RESET = "\033[0m", TEAL = "\033[38;2;54;199;176m";
    const std::string WHITE = "\033[38;2;241;242;246m", GREY = "\033[38;2;160;165;181m";
    const std::string BLUE = "\033[38;2;112;167;245m", AMBER = "\033[38;2;242;183;91m";
    const std::string GREEN = "\033[38;2;66;201;138m", RED = "\033[38;2;239;114;122m";

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

    void printFormattedReport(const std::string& report) {
        std::istringstream stream(report);
        std::string line, section;

        while (std::getline(stream, line)) {
            if (line == "Data:" || line == "Work items:" || line == "History:") {
                section = line;
                std::cout << "\n" << TEAL << "\033[1m" << line << RESET << "\n";
            } else if (line.find("===") == 0) {
                std::cout << TEAL << line << RESET << "\n";
            } else if (line.find("State: ") == 0) {
                std::string state = line.substr(7);
                std::cout << GREY << "State: " << statusColor(state) << state << RESET << "\n";
            } else if (section == "Data:" && line.find(" = ") != std::string::npos) {
                std::size_t pos = line.find(" = ");
                std::string value = line.substr(pos + 3);
                std::cout << GREY << line.substr(0, pos) << " = " << (value.empty() ? AMBER : WHITE) << (value.empty() ? "Not provided" : value) << RESET << "\n";
            } else if (section == "Work items:" && line.find(": ") != std::string::npos) {
                std::size_t pos = line.find(": ");
                std::string details = line.substr(pos + 2);
                std::size_t space = details.find(' ');
                std::string state = details.substr(0, space);
                std::cout << WHITE << line.substr(0, pos + 2) << statusColor(state) << state << GREY << (space == std::string::npos ? "" : details.substr(space)) << RESET << "\n";
            } else if (section == "History:" && !line.empty()) {
                std::cout << GREY << line << RESET << "\n";
            } else {
                std::cout << line << "\n";
            }
        }
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
            std::cout << GREY << "Current Status: " << statusColor(status) << status << RESET << "\n\n";
            TerminalUI::showDivider();
            printFormattedReport(facade.generateReport(id));
            TerminalUI::showDivider();
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

            std::vector<std::string> choices = items;
            choices.push_back("Back");
            int choice = TerminalUI::selectOption("SELECT WORK ITEM", choices, false);

            if (choice == static_cast<int>(items.size())) {
                return;
            }
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
        TerminalUI::showHeader("WORK ITEM | " + name);
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
            showResult("Issue submitted to escalation.");
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

        items.push_back("Back");
        int choice = TerminalUI::selectOption("SELECT WORK ITEM FOR ISSUE", items);
        if (choice == static_cast<int>(items.size()) - 1) {
            return;
        }

        std::string type = promptText("Issue type: ", true);
        std::string description = promptText("Description: ", true);
        int severity = promptSeverity();
        runWithStyledOutput([&]() {
            facade.reportIssue(id, items[choice], type, description, severity);
        });
        showResult("Issue submitted to escalation.");
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
