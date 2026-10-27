#include "../include/Iterator.h"
#include "../include/composite.h"
#include "../include/State.h"

#include <sstream>
#include <string>

ReportGenerator::ReportGenerator(AuditHistory* history){
    this->history = history;
}


//summary for a running instance
std::string ReportGenerator::progressReport(const WorkflowInstance& instance){
    int total = 0;
    int done = 0;

    // walk every element under the root by depth
    WorkGroup* root = instance.getRoot();
    if(root != nullptr){
        WorkIterator<WorkElement>* it = root->createIterator();
        for(it->first(); !it->isDone(); it->next()){
            WorkElement* element = it->current();

            // only tasks are counted thegroups are just containers
            if(element->asGroup() == nullptr){
                total++;
                if(element->isComplete()){
                    done++;
                }
            }
        }

        // createIterator owns the iterator so we delete it
        delete it;
    }

    // build the summary line
    std::ostringstream report;
    report << "Instance " << instance.getId() << ": " << instance.getStateName()
           << ", " << done << "/" << total << " tasks complete";
    return report.str();
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
