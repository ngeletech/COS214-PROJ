#ifndef WORKFLOWSCREENS_H
#define WORKFLOWSCREENS_H

#include "TividyFacade.h"

class WorkflowScreens {
private:
    TividyFacade& facade;
    void showAvailableWork(int instanceId);

public:
    explicit WorkflowScreens(TividyFacade& facade);
    void showOverview(int instanceId);
};

#endif
