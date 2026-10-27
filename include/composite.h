#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <string>
#include <vector>
#include "Iterator.h"


// Forward declarations
class Issue;
class Participant;
class WorkData;
class WorkflowInstance;
class WorkGroup;

/** @note 
    
    Not in the uml parent, setParent(), asGroup(), getItems(), WorkGroup::isAvailable()
    and the ParallelGroup and SequentialGroup constructors.
 
 */

/** @brief Status of a work element. */
enum class WorkItemStatus {
    CREATED,
    AVAILABLE,
    ASSIGNED,
    STARTED,
    COMPLETED,
    REJECTED,
    CANCELLED
};

/** @brief Component: a task or a group of tasks. */
class WorkElement {
private:
    std::string name;
    WorkItemStatus status;
    WorkflowInstance* instance;

protected:
    WorkGroup* parent;

public:
    WorkElement(const std::string& name);
    virtual ~WorkElement();

    virtual void execute() = 0;
    virtual void complete() = 0;
    virtual bool isComplete() const = 0;

    /** @brief Does nothing by default. */
    virtual void assignTo(Participant* participant);

    virtual WorkData getPayload() const;
    virtual bool isAvailable() const;
    virtual std::string getName() const;
    virtual WorkItemStatus getStatus() const;
    virtual void setStatus(WorkItemStatus status);
    virtual WorkflowInstance* getInstance() const;
    virtual void setInstance(WorkflowInstance* instance);
    virtual void setParent(WorkGroup* parent);

    /** @brief Returns nullptr if this isn't a group. */
    virtual WorkGroup* asGroup();

    // Not in the UML, but in the implementation spec I added them regardless
    // should you choose to use them 
    virtual Participant* getAssignee() const;
    virtual bool requiresAssignee() const;
    virtual int getPriority() const;
};

/** @brief Leaf: a single task. execute() is the Template Method. */
class Task : public WorkElement {
protected:
    Participant* assignee;

    /** @brief True if Assigned, or Available when no assignee is needed. */
    bool checkReady() const;

    /** @brief The one step each kind of task fills in. */
    virtual void doWork() = 0;

    /** @brief Publishes an ElementExecuted event. */
    void notifyListeners();

public:
    Task(const std::string& name);

    void execute() override;
    void complete() override;
    bool isComplete() const override;
    void assignTo(Participant* participant) override;
    Participant* getAssignee() const override;
    bool requiresAssignee() const override;

    void reportIssue(Issue issue);
};

/** @brief Composite: a group of work elements. */
class WorkGroup : public WorkElement, public WorkIterable<WorkElement> {
private:
    std::vector<WorkElement*> items;

public:
    WorkGroup(const std::string& name);

    /** @brief Deletes every item. */
    ~WorkGroup();

    void add(WorkElement* item);
    const std::vector<WorkElement*>& getItems() const;
    virtual bool canStartChild(const std::string& childName);

    void execute() override;
    void complete() override;
    bool isComplete() const override;
    bool isAvailable() const override;
    void assignTo(Participant* participant) override;
    WorkGroup* asGroup() override;
    WorkIterator<WorkElement>* createIterator() override;
};

/** @brief Items can run at the same time. */
class ParallelGroup : public WorkGroup {
public:
    ParallelGroup(const std::string& name);
    bool canStartChild(const std::string& childName) override;
};

/** @brief Items run one after another. */
class SequentialGroup : public WorkGroup {
public:
    SequentialGroup(const std::string& name);
    bool canStartChild(const std::string& childName) override;
};



#endif // COMPOSITE_H


