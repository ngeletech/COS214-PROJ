#include "WorkflowScreens.h"
#include "TerminalUI.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace {
    const std::string RESET = "\033[0m";
    const std::string TEAL  = "\033[38;2;54;199;176m";
    const std::string WHITE = "\033[38;2;241;242;246m";
    const std::string GREY  = "\033[38;2;160;165;181m";
    const std::string BLUE  = "\033[38;2;112;167;245m";
    const std::string AMBER = "\033[38;2;242;183;91m";
    const std::string GREEN = "\033[38;2;66;201;138m";
    const std::string RED   = "\033[38;2;239;114;122m";

    std::string statusColor(const std::string& status) {
        if (status == "Running" || status == "completed")
            return GREEN;

        if (status == "started" || status == "assigned")
            return BLUE;

        if (status == "available")
            return AMBER;

        if (status == "rejected" || status == "cancelled" ||
            status == "Failed")
            return RED;

        return GREY;
    }

    void printFormattedReport(const std::string& report) {
        std::istringstream stream(report);
        std::string line;
        std::string section;

        while (std::getline(stream, line)) {
            if (line == "Data:" ||
                line == "Work items:" ||
                line == "History:") {

                section = line;
                std::cout << "\n" << TEAL << "\033[1m"
                          << line << "\033[0m\n";
                continue;
            }

            if (line.find("===") == 0) {
                std::cout << TEAL << line << RESET << "\n";
                continue;
            }

            if (line.find("State: ") == 0) {
                std::string value = line.substr(7);
                std::cout << GREY << "State: "
                          << statusColor(value) << value
                          << RESET << "\n";
                continue;
            }

            if (section == "Data:") {
                std::size_t pos = line.find(" = ");

                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 3);

                    std::cout << GREY << key << " = "
                              << (value.empty() ? AMBER : WHITE)
                              << (value.empty() ? "Not provided" : value)
                              << RESET << "\n";
                    continue;
                }
            }

            if (section == "Work items:") {
                std::size_t pos = line.find(": ");

                if (pos != std::string::npos) {
                    std::string name = line.substr(0, pos + 2);
                    std::string details = line.substr(pos + 2);

                    std::size_t space = details.find(' ');
                    std::string status = details.substr(0, space);
                    std::string assignee =
                        space == std::string::npos
                            ? ""
                            : details.substr(space);

                    std::cout << WHITE << name
                              << statusColor(status) << status
                              << GREY << assignee
                              << RESET << "\n";
                    continue;
                }
            }

            if (section == "History:" && !line.empty()) {
                std::cout << GREY << line << RESET << "\n";
                continue;
            }

            std::cout << line << "\n";
        }
    }
}

WorkflowScreens::WorkflowScreens(TividyFacade& facade) : facade(facade) {}

void WorkflowScreens::showOverview(int instanceId) {
    bool viewing = true;

    while (viewing) {
        TerminalUI::clearScreen();
        TerminalUI::showHeader("WORKFLOW OVERVIEW | INSTANCE " + std::to_string(instanceId));

        try {
            std::string status = facade.trackStatus(instanceId);
            std::cout << GREY << "Current Status: " << statusColor(status) << status << RESET << "\n\n";
            TerminalUI::showDivider();
            printFormattedReport(facade.generateReport(instanceId));
            TerminalUI::showDivider();
        } catch (const std::exception& error) {
            TerminalUI::showError(error.what());
            TerminalUI::waitForContinue();
            return;
        }

        std::vector<std::string> options = {"Refresh Overview", "View Available Work", "Back"};

        const int choice = TerminalUI::selectOption("WORKFLOW OVERVIEW | INSTANCE " + std::to_string(instanceId), options, false);
        switch (choice) {
            case 0:
                break;
            case 1:
                showAvailableWork(instanceId);
                break;
            case 2:
                viewing = false;
                break;
            default:
                TerminalUI::showError("Invalid menu option.");
                TerminalUI::waitForContinue();
                break;
        }
    }
}

void WorkflowScreens::showAvailableWork(int instanceId) {
    TerminalUI::clearScreen();
    TerminalUI::showHeader("AVAILABLE WORK | INSTANCE " + std::to_string(instanceId));

    try {
        const std::vector<std::string> items = facade.getAvailableWork(instanceId);
        if (items.empty()) {
            TerminalUI::showInfo("No available work items for this instance.");
        } else {
            std::cout << "Work items currently available for interaction:\n\n";
            for (std::size_t i = 0; i < items.size(); ++i) {
                TerminalUI::showMenuOption(static_cast<int>(i + 1), items[i]);
            }
        }
    } catch (const std::exception& error) {
        TerminalUI::showError(error.what());
    }

    TerminalUI::showDivider();
    TerminalUI::waitForContinue();
}
