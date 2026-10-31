#ifndef TERMINALUI_H
#define TERMINALUI_H

#include <string>

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
    void waitForContinue();
}

#endif
