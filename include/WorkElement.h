#ifndef WORKELEMENT_H
#define WORKELEMENT_H

#include <string>

// Forward declarations
class WorkflowInstance;
class Participant;
struct WorkData;
enum class WorkItemStatus {
    CREATED,
    AVAILABLE,
    ASSIGNED,
    STARTED,
    COMPLETED,
    REJECTED,
    CANCELLED
};

/**
 * @class WorkElement
 * @brief Abstract base class defining the Template Method for executing work elements.
 */
class WorkElement {
protected:
    std::string name;                   /**< Name of the work element. */
    WorkItemStatus status;              /**< Current execution status. */
    WorkflowInstance* instance;         /**< Pointer to the owning workflow instance. */

    /**
     * @brief Protected primitive step to be implemented by task subclasses.
     */
    virtual void doWork() = 0;

public:
    /**
     * @brief Constructor for WorkElement.
     * @param name Name of the work element.
     */
    WorkElement(const std::string& name);

    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkElement();

    /**
     * @brief Template Method defining the standard execution algorithm lifecycle.
     */
    virtual void execute();

    /**
     * @brief Finalizes the completion of the work element.
     */
    virtual void complete();

    /**
     * @brief Checks if the work element is complete.
     * @return true if complete, false otherwise.
     */
    virtual bool isComplete() const;

    /**
     * @brief Assigns the work element to a participant.
     * @param participant Pointer to the assigned participant.
     */
    virtual void assignTo(Participant* participant);

    /**
     * @brief Gets the payload/data associated with the work element.
     * @return WorkData Payload data structure.
     */
    virtual WorkData getPayload() const;

    /**
     * @brief Checks if the element is available for execution.
     * @return true if available, false otherwise.
     */
    virtual bool isAvailable() const;

    /**
     * @brief Gets the name of the work element.
     * @return std::string Name.
     */
    std::string getName() const;

    /**
     * @brief Gets the current work item status.
     * @return WorkItemStatus Current status enum.
     */
    WorkItemStatus getStatus() const;

    /**
     * @brief Sets the work item status.
     * @param status New status to assign.
     */
    void setStatus(WorkItemStatus status);

    /**
     * @brief Gets the attached workflow instance.
     * @return WorkflowInstance* Pointer to instance.
     */
    WorkflowInstance* getInstance() const;

    /**
     * @brief Sets the parent workflow instance.
     * @param instance Pointer to workflow instance.
     */
    void setInstance(WorkflowInstance* instance);
};

#endif // WORKELEMENT_H