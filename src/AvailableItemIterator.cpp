#include "../include/Iterator.h"
#include "../include/composite.h"

AvailableItemIterator::AvailableItemIterator(WorkElement* root) : inner(root) {
    first();
}

void AvailableItemIterator::first(){
    inner.first();
    // skip everything that cant be be unlocked yet
    while(!inner.isDone() && !inner.current()->isAvailable()){
        inner.next();
    }
}

void AvailableItemIterator::next(){
    inner.next();
    while(!inner.isDone() && !inner.current()->isAvailable()){
        inner.next();
    }
}

bool AvailableItemIterator::isDone(){
    return inner.isDone();
}

WorkElement* AvailableItemIterator::current(){
    return inner.current();
}
