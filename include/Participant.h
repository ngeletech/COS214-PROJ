#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <map>
#include <string>

class ExternalServiceConnector;

/** A person or system that work can be assigned to. Does not own its connector. */
class Participant {
private:
    int id;
    std::string name;
    std::string role;
    int workload;
    ExternalServiceConnector* connector;
    std::map<std::string, std::string> attributes;
public:
    Participant(int id, const std::string& name, const std::string& role,
                ExternalServiceConnector* connector = nullptr);

    int getId() const;
    const std::string& getName() const;
    const std::string& getRole() const;
    int getWorkload() const;
    void incrementWorkload();
    void decrementWorkload();                 // never goes below zero
    ExternalServiceConnector* getConnector() const;

    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;   // "" if missing
    bool hasAttribute(const std::string& key) const;
};

#endif