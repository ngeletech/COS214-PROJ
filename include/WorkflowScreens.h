#ifndef WORKFLOWSCREENS_H
#define WORKFLOWSCREENS_H
#include "TividyFacade.h"
#include <string>
class WorkflowScreens {
private:
    TividyFacade& facade;
    void showAvailableWork(int instanceId);
    void interactWithItem(int instanceId, const std::string& name);
    void reportIssue(int instanceId);
    void showHistory(int instanceId);
    void undoLastCommand();
public:
    explicit WorkflowScreens(TividyFacade& facade);
    void showOverview(int instanceId);
};
#endif
