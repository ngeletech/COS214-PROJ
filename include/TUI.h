#ifndef TUI_H
#define TUI_H

#include "TividyFacade.h"
#include <vector>

class TUI {
private:
    // Reference to the workflow engine's Facade.
    TividyFacade& facade;

    // Stores workflow instance IDs created during this session.
    std::vector<int> activeInstances;

    // Displays the main navigation menu.
    void showMainMenu();

    // Displays the screen for starting a new workflow.
    void showStartWorkflow();

    // Create a new sequential workflow definition at runtime.
    void showCreateWorkflow();

    // Displays workflows currently active in this session.
    void showActiveWorkflows();

    // Opens the overview screen for a selected workflow.
    void showWorkflowOverview(int instanceId);

public:
    // Initialises the TUI using the existing Facade.
    explicit TUI(TividyFacade& facade);

    // Starts the interactive terminal interface.
    void run();
};

#endif