#include "TividyFacade.h"
#include "TUI.h"
#include "Scenarios.h"

int main() {

    // Create the workflow engine.
    TividyFacade facade;

    // Register the two industry workflows.
    setupFoodCateringScenario(facade);
    setupTelecomScenario(facade);

    // Create and launch the terminal interface.
    TUI tui(facade);
    tui.run();

    return 0;
}