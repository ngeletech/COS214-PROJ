#include "TerminalUI.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace TerminalUI {

    namespace {
        // Terminal colour codes.
        const std::string RESET = "\033[0m";
        const std::string TEAL = "\033[38;2;54;199;176m";
        const std::string WHITE = "\033[38;2;241;242;246m";
        const std::string GREY = "\033[38;2;160;165;181m";
        const std::string BLUE = "\033[38;2;112;167;245m";
        const std::string AMBER = "\033[38;2;242;183;91m";
        const std::string RED = "\033[38;2;239;114;122m";
        const std::string GREEN = "\033[38;2;66;201;138m";
    }

    /**
     * @brief Enables ANSI colour support in the terminal.
     *
     * Windows requires virtual terminal processing to be enabled.
     * macOS and Linux terminals generally support ANSI sequences natively.
     */
    void enableColors() {

        #ifdef _WIN32

        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;

        if (GetConsoleMode(console, &mode)) {
            SetConsoleMode(
                console,
                mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
            );
        }

        #endif
    }

    /**
     * @brief Clears the terminal screen and moves the cursor to the top.
     */
    void clearScreen() {
        std::cout << "\033[2J\033[H";
        std::cout.flush();
    }

    /**
     * @brief Displays the TIVIDY application header.
     * @param subtitle The title of the current screen.
     */
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

    /**
     * @brief Displays a horizontal divider.
     */
    void showDivider() {
        std::cout << GREY
                  << "----------------------------------------\n"
                  << RESET;
    }

    /**
     * @brief Displays a success message.
     * @param message The success message to display.
     */
    void showSuccess(const std::string& message) {
        std::cout << GREEN << "[SUCCESS] " << RESET
                  << WHITE << message << RESET << "\n";
    }

    /**
     * @brief Displays an error message.
     * @param message The error message to display.
     */
    void showError(const std::string& message) {
        std::cout << RED << "[ERROR] " << RESET
                  << WHITE << message << RESET << "\n";
    }

    /**
     * @brief Displays an informational message.
     * @param message The information to display.
     */
    void showInfo(const std::string& message) {
        std::cout << BLUE << "[INFO] " << RESET
                  << WHITE << message << RESET << "\n";
    }

    /**
     * @brief Displays a numbered menu option.
     * @param number The menu option number.
     * @param label The description of the option.
     */
    void showMenuOption(int number, const std::string& label) {
        std::cout << GREY << "  " << number << ". " << RESET
                  << WHITE << label << RESET << "\n";
    }

    /**
     * @brief Prompts the user to enter an integer.
     * @param message The prompt displayed to the user.
     * @return The integer entered by the user.
     */
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

    /**
     * @brief Waits for the user to press ENTER before continuing.
     */
    void waitForContinue() {
        std::cout << "\n" << GREY
                  << "Press ENTER to continue..."
                  << RESET;

        std::cin.get();
    }

}
