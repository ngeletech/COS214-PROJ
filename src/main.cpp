#include "TividyFacade.h"
#include "scenarios.h"
#include "TUI.h"

int main() {

    // Create the workflow engine facade
    TividyFacade facade;

    // Register both industry workflows
    setupFoodCateringScenario(facade);
    setupTelecomScenario(facade);

    // Enable terminal colours
    TerminalUI::enableColors();

    // Start the TUI
    TUI tui(facade);
    tui.run();

    return 0;
}
