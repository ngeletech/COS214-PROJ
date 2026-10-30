#ifndef SCENARIOS_H
#define SCENARIOS_H

#include "TividyFacade.h"

// Set up and register Scenario 1
void setupFoodCateringScenario(TividyFacade& facade);

// Set up and register Scenario 2
void setupTelecomScenario(TividyFacade& facade);

// execute live demo for Scenario 1
void runFoodCateringDemo(TividyFacade& facade);

// execute live demo for Scenario 2
void runTelecomDemo(TividyFacade& facade);

#endif // SCENARIOS_H