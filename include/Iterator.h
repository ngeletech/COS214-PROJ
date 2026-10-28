#ifndef ITERATOR_H
#define ITERATOR_H

#include <string>
#include <vector>

// Forward declarations
class WorkElement;
class WorkflowInstance;

/** @brief Iterator interface. */
template <typename T>
class WorkIterator {
public:
    /** @brief Virtual destructor. */
    virtual ~WorkIterator() {}

    /** @brief Moves to the first item. */
    virtual void first() = 0;

    /** @brief Moves to the next item. */
    virtual void next() = 0;

    /** @brief Returns true at the end. */
    virtual bool isDone() = 0;

    /** @brief Returns the current item. */
    virtual T* current() = 0;
};

/** @brief Aggregate interface. */
template <typename T>
class WorkIterable {
public:
    /** @brief Virtual destructor. */
    virtual ~WorkIterable() {}

    /** @brief Returns a new iterator. The caller deletes it. */
    virtual WorkIterator<T>* createIterator() = 0;
};

/** @brief One entry in the audit log. */
class AuditEntry {
private:
    int instanceId;
    std::string eventType;
    std::string details;

public:
    /** @brief Creates an entry. Put any element name in details. */
    AuditEntry(int instanceId, const std::string& eventType, const std::string& details);

    /** @brief Returns the instance id. */
    int getInstanceId() const;

    /** @brief Returns the event type. */
    std::string getEventType() const;

    /** @brief Returns the details. */
    std::string getDetails() const;
};

/** @brief Iterates over the audit log. */
class AuditEntryIterator : public WorkIterator<const AuditEntry> {
private:
    const std::vector<AuditEntry>* entries;
    int index;

public:
    /** @brief Starts at the first entry. */
    AuditEntryIterator(const std::vector<AuditEntry>* entries);

    void first() override;
    void next() override;
    bool isDone() override;
    const AuditEntry* current() override;
};

/** @brief The audit log. */
class AuditHistory : public WorkIterable<const AuditEntry> {
private:
    std::vector<AuditEntry> entries;

public:
    /** @brief Adds an entry. */
    void append(const AuditEntry& entry);

    WorkIterator<const AuditEntry>* createIterator() override;
};

/** @brief Visits every element, depth-first. */
class DepthFirstIterator : public WorkIterator<WorkElement> {
private:
    WorkElement* start;
    std::vector<WorkElement*> stack;
    WorkElement* cur;

public:
    /** @brief Starts at the root. */
    DepthFirstIterator(WorkElement* root);

    void first() override;
    void next() override;
    bool isDone() override;
    WorkElement* current() override;
};

/** @brief Visits only available elements. */
class AvailableItemIterator : public WorkIterator<WorkElement> {
private:
    DepthFirstIterator inner;

public:
    /** @brief Starts at the first available element. */
    AvailableItemIterator(WorkElement* root);

    void first() override;
    void next() override;
    bool isDone() override;
    WorkElement* current() override;
};

/** @brief Builds progress and audit reports. */
class ReportGenerator {
private:
    AuditHistory* history;

public:
    /** @brief Uses the given audit log. */
    ReportGenerator(AuditHistory* history);

    /** @brief Returns an instance's progress. */
    std::string progressReport(const WorkflowInstance& instance);

    /** @brief Returns an instance's audit entries. */
    std::string auditReport(int instanceId);
};

#endif // ITERATOR_H
