#include "../include/composite.h"

#include <string>

ParallelGroup::ParallelGroup(const std::string& name) : WorkGroup(name) {

}

// every child may run at the same time
bool ParallelGroup::canStartChild(const std::string& childName){
    (void)childName;
    return true;
}
