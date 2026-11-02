#ifndef TERMINALUI_H
#define TERMINALUI_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace TerminalUI {

    // Terminal setup
    void enableColors();
    void clearScreen();

    // Interface layout
    void showHeader(const std::string& subtitle);
    void showDivider();

    // Messages
    void showSuccess(const std::string& message);
    void showError(const std::string& message);
    void showInfo(const std::string& message);

    // Menu
    void showMenuOption(int number, const std::string& label);

    // User input
    int promptInt(const std::string& message);
    int selectOption(const std::string& prompt, const std::vector<std::string>& options, bool clear = true);
    void waitForContinue();

    // formatting
    void runWithStyledOutput(const std::function<void()>& action, int delayMs = 90);
}

#endif
