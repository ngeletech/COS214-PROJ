#include "../include/Iterator.h"

#include <vector>

// entries can only be appended never removed
void AuditHistory::append(const AuditEntry& entry){
    entries.push_back(entry);
}

// the caller deletes the iterator
WorkIterator<const AuditEntry>* AuditHistory::createIterator(){
    return new AuditEntryIterator(&entries);
}
