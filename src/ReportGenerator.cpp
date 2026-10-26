#include "../include/Iterator.h"
#include "../include/composite.h"

#include <string>

ReportGenerator::ReportGenerator(AuditHistory* history){
    this->history = history;
}

// to be implemented later once dependencies are met
std::string ReportGenerator::progressReport(const WorkflowInstance& instance){
    return "";
}

// one line per entry for this instance 
std::string ReportGenerator::auditReport(int instanceId){
    std::string report = "";
    if(history == nullptr){
        return report;
    }

    WorkIterator<const AuditEntry>* it = history->createIterator();
    for(it->first(); !it->isDone(); it->next()){
        const AuditEntry* entry = it->current();
        if(entry->getInstanceId() == instanceId){
            report += entry->getEventType();
            if(!entry->getDetails().empty()){
                report += ": " + entry->getDetails();
            }
            report += "\n";
        }
    }
    delete it;

    return report;
}
