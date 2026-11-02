#include "TerminalUI.h"

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <thread>
#include <chrono>
#include <exception>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
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

        std::string messageColor(const std::string& label) {
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
            if (message.find("is not ready to execute") != std::string::npos) {
                return;
            }

            std::size_t end = message.find(']');

            if (message[0] == '[' && end != std::string::npos) {
                std::string label = message.substr(0, end + 1);
                std::string description = message.substr(end + 1);

                std::cout << messageColor(label) << label << RESET << WHITE << description << RESET << "\n";
            }
            else {
                std::cout << GREY << message << RESET << "\n";
            }

            std::cout.flush();
        }
        #ifdef _WIN32
                int readKey() {
                    int key = _getch();
                    if (key == 0 || key == 224) {
                        int arrow = _getch();
                        if (arrow == 72) return 1; // Up
                        if (arrow == 80) return 2; // Down
                    }
                    if (key == 13) return 3; // Enter
                    return 0;
                }
        #else
                class RawMode {
                private:
                    termios original;
                    bool enabled;
                public:
                    RawMode() : enabled(false) {
                        if (tcgetattr(STDIN_FILENO, &original) != 0) {
                            throw std::runtime_error("Cannot read terminal settings.");
                        }
                        termios raw = original;
                        raw.c_lflag &= ~(ICANON | ECHO);

                        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
                            throw std::runtime_error("Cannot enable raw terminal mode.");
                        }
                        enabled = true;
                    }

                    ~RawMode() {
                        if (enabled) {
                            tcsetattr(STDIN_FILENO, TCSANOW, &original);
                        }
                    }
                };

                int readKey() {
                    unsigned char key;

                    if (read(STDIN_FILENO, &key, 1) != 1)
                        return 0;

                    if (key == '\n' || key == '\r')
                        return 3;

                    if (key == 27) {
                        unsigned char sequence[2];

                        if (read(STDIN_FILENO, &sequence[0], 1) != 1)
                            return 0;

                        if (read(STDIN_FILENO, &sequence[1], 1) != 1)
                            return 0;

                        if (sequence[0] == '[') {
                            if (sequence[1] == 'A') return 1;
                            if (sequence[1] == 'B') return 2;
                        }
                    }

                    return 0;
                }

        #endif
    }

    void runWithStyledOutput(const std::function<void()>& action, int delayMs) {
        if (delayMs < 0) {
            delayMs = 0;
        }

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

            if (!line.empty() && delayMs > 0) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(delayMs)
                );
            }
        }

        if (error) {
            std::rethrow_exception(error);
        }
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
    int selectOption(const std::string& prompt, const std::vector<std::string>& options, bool clear) {
        if (options.empty()) {
            throw std::invalid_argument("Selection options cannot be empty.");
        }

        #ifndef _WIN32
            RawMode rawMode;
        #endif

        int selected = 0;
        bool firstDraw = true;

        const std::size_t menuLines = options.size() + 4;

        while (true) {
            if (clear) {
                clearScreen();
            }
            else if (!firstDraw) {
                // Move the cursor back to the start of the menu.
                std::cout << "\033[" << menuLines << "A";
            }
            if (clear) {
                showHeader(prompt);
            }
            else {
                std::cout << TEAL << prompt << RESET << "\n\n";
            }
            for (std::size_t i = 0; i < options.size(); ++i) {
                // Clear this line before redrawing it.
                std::cout << "\033[2K";
                if (static_cast<int>(i) == selected) {
                    std::cout << GREEN << "  > " << (i + 1) << ". " << options[i] << RESET << "\n";
                }
                else {
                    std::cout << GREY << "    " << (i + 1) << ". " << options[i] << RESET << "\n";
                }
            }

            std::cout << "\n\033[2K" << GREY << "Use UP/DOWN arrows to navigate, ENTER to select." << RESET << "\n";

            std::cout.flush();

            firstDraw = false;

            int key = readKey();

            if (key == 1) {
                selected = (selected - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size());
            }
            else if (key == 2) {
                selected = (selected + 1) % static_cast<int>(options.size());
            }
            else if (key == 3) {
                return selected;
            }
        }
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
        std::cout << GREY << "  " << number << ". " << RESET << WHITE << label << RESET << "\n";
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
        std::cout << "\n" << GREY << "Press ENTER to continue..."<< RESET;
        std::cin.get();
    }

}
