#include "../include/composite.h"

#include <string>
#include <vector>

SequentialGroup::SequentialGroup(const std::string& name) : WorkGroup(name) {

}

// a child may start only once every child before it is complete
bool SequentialGroup::canStartChild(const std::string& childName){
    const std::vector<WorkElement*>& children = getItems();
    int size = children.size();
    for(int i = 0; i < size; i++){
        if(children[i]->getName() == childName){
            return true;
        }
        if(!children[i]->isComplete()){
            return false;
        }
    }
    return false;
}
