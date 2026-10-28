#include "../include/composite.h"

#include <string>

 WorkGroup::WorkGroup(const std::string& name) : WorkElement(name) {

 }


 WorkGroup::~WorkGroup(){
    int size = items.size();
    for(int i = 0; i < size; i++){
        delete items[i];
    }
 }

void  WorkGroup::add(WorkElement* item){
    items.push_back(item);
    item->setParent(this);
    item->setInstance(getInstance());
 }

const std::vector<WorkElement*>& WorkGroup::getItems() const {
    return items;
}

bool WorkGroup::canStartChild(const std::string& childName){
    (void)childName;
    return true;
}

bool WorkGroup::isComplete() const {
    int size = items.size();
    for(int i = 0; i < size; i++){
        if(!items[i]->isComplete()){
            return false;
        }
    }
    return true;
}

void WorkGroup::execute(){
    int size = items.size();
    for(int i = 0; i < size; i++){
        if(canStartChild(items[i]->getName())){
            items[i]->execute();
        }
    }
}


void WorkGroup::complete(){
    if(!isComplete() || getStatus() == WorkItemStatus::COMPLETED){
        return;
    }
    setStatus(WorkItemStatus::COMPLETED);
    if(parent != nullptr){
        parent->complete();
    }
}

bool WorkGroup::isAvailable() const {
    return false;
}

//no op, we dont assign groups
void WorkGroup::assignTo(Participant* participant){
    (void)participant;
}

WorkGroup* WorkGroup::asGroup(){
    return this;
}

WorkIterator<WorkElement>* WorkGroup::createIterator(){
    return new DepthFirstIterator(this);
}