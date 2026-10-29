#include "WorkData.h"

void WorkData::addData(const std::string& key, const std::string& value) {
    data[key] = value;
}
    
const std::map<std::string, std::string>& WorkData::getData() const {
    return data;
}
