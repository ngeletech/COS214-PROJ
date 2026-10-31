#include "../include/Iterator.h"

#include <vector>

AuditEntryIterator::AuditEntryIterator(const std::vector<AuditEntry>* entries){
    this->entries = entries;
    index = 0;
}

void AuditEntryIterator::first(){
    index = 0;
}

void AuditEntryIterator::next(){
    index++;
}

bool AuditEntryIterator::isDone(){
    if(entries == nullptr){
        return true;
    }
    int size = entries->size();
    return index >= size;
}

const AuditEntry* AuditEntryIterator::current(){
    if(isDone()){
        return nullptr;
    }
    return &(*entries)[index];
}
