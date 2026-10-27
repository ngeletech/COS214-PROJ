#ifndef WORKDATA_H
#define WORKDATA_H

#include <string>
#include <map>


class WorkData {
private:
    std::map<std::string, std::string> data;
public:
    void addData(const std::string& key, const std::string& value);
    const std::map<std::string, std::string>& getData() const;
};

#endif
