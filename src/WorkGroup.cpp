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