#ifndef ISSUE_H
#define ISSUE_H
#include <string>

class Issue {
    std::string message;
public:
    explicit Issue(const std::string& m = "") : message(m) {}
    const std::string& getMessage() const { return message; }
};
#endif