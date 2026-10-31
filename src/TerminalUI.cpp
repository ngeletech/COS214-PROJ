#include "TerminalUI.h"

#include <iostream>
#include <windows.h>

namespace TerminalUI {

    namespace {

        // Midnight Teal colour palette
        const std::string RESET = "\033[0m";

        // Main interface colours
        const std::string TEAL  = "\033[38;2;54;199;176m";
        const std::string WHITE = "\033[38;2;241;242;246m";
        const std::string GREY  = "\033[38;2;160;165;181m";

        // Workflow status colours
        const std::string BLUE  = "\033[38;2;112;167;245m";
        const std::string AMBER = "\033[38;2;242;183;91m";
        const std::string RED   = "\033[38;2;239;114;122m";
        const std::string GREEN = "\033[38;2;66;201;138m";
    }

    // Enable ANSI colour support in Windows terminal
    void enableColors() {

        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;

        if (GetConsoleMode(console, &mode)) {
            SetConsoleMode(
                console,
                mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
            );
        }
    }

    // Clear terminal screen
    void clearScreen() {
        std::cout << "\033[2J\033[H";
        std::cout.flush();
    }

    // Display application header
    void showHeader(const std::string& subtitle) {

        std::cout << "\n";

        std::cout << TEAL;
        std::cout << "========================================\n";
        std::cout << "                 TIVIDY                 \n";
        std::cout << "       WORKFLOW MANAGEMENT SYSTEM       \n";
        std::cout << "========================================\n";
        std::cout << RESET;

        std::cout << GREY << subtitle << RESET << "\n\n";
    }

    // Display divider
    void showDivider() {
        std::cout << GREY
                  << "----------------------------------------\n"
                  << RESET;
    }

    // Display successful operation
    void showSuccess(const std::string& message) {

        std::cout << GREEN << "[SUCCESS] " << RESET
                  << WHITE << message << RESET << "\n";
    }

    // Display error message
    void showError(const std::string& message) {

        std::cout << RED << "[ERROR] " << RESET
                  << WHITE << message << RESET << "\n";
    }

    // Display information message
    void showInfo(const std::string& message) {

        std::cout << BLUE << "[INFO] " << RESET
                  << WHITE << message << RESET << "\n";
    }

    // Display menu option
    void showMenuOption(int number, const std::string& label) {

        std::cout << GREY << "  " << number << ". " << RESET
                  << WHITE << label << RESET << "\n";
    }

    // Read and validate integer input
    int promptInt(const std::string& message) {

        int value;

        while (true) {

            std::cout << TEAL << message << RESET;

            if (std::cin >> value) {
                std::cin.ignore(10000, '\n');
                return value;
            }

            std::cin.clear();
            std::cin.ignore(10000, '\n');

            showError("Please enter a valid number.");
        }
    }

    // Wait for user before continuing
    void waitForContinue() {

        std::cout << "\n"
                  << GREY << "Press ENTER to continue..."
                  << RESET;

        std::cin.get();
    }
}