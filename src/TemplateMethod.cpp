#include "../include/TemplateMethod.h"

#include <iostream>

// 1. UserTask Implementation
UserTask::UserTask(const std::string& name) : Task(name) {}

void UserTask::doWork() {
    std::cout << "[UserTask]: Executing manual step '" << name << "' assigned to human participant" << std::endl;
}


// 2. ServiceTask Implementation
ServiceTask::ServiceTask(const std::string& name) : Task(name) {}

void ServiceTask::doWork() {
    std::cout << "[ServiceTask]: Invoking external service endpoint '" << name << "'." << std::endl;
}

// 3. ApprovalTask Implementation
ApprovalTask::ApprovalTask(const std::string& name) : Task(name), approved(false) {}

void ApprovalTask::recordDecision(bool isApproved) {
    this->approved = isApproved;
    std::cout << "[ApprovalTask]: Decision recorded for '" << getName() 
              << "': " << (approved ? "APPROVED" : "REJECTED") << std::endl;
}

void ApprovalTask::doWork() {
    std::cout << "[ApprovalTask]: Processing approval decision for '" << getName() << "'." << std::endl;
    if (approved) {
        std::cout << "[ApprovalTask]: Task '" << getName() << "' has been approved." << std::endl;
    } else {
        std::cout << "[ApprovalTask]: Task '" << getName() << "' has been rejected." << std::endl;
    }
}

// 4. ScriptTask Implementation
ScriptTask::ScriptTask(const std::string& name, const std::string& key) : Task(name, key) {}

void ScriptTask::doWork() {
    std::cout << "[ScriptTask]: Executing script '" << name << "' with key '" << key << "'." << std::endl;
}

