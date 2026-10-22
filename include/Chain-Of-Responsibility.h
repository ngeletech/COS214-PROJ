#ifndef CHAINOFRESPONSIBILITY_H
#define CHAINOFRESPONSIBILITY_H

class Participant;
class Issue;

/**
 * @brief Abstract base class implementing the Chain of Responsibility pattern.
 *
 * EscalationHandler represents a handler in the escalation chain.
 * If an issue exceeds the handler's authority level, it is forwarded
 * to the next handler in the chain.
 */
class EscalationHandler {
protected:

    /**
     * @brief Pointer to the next handler in the escalation chain.
     */
    EscalationHandler* next;

    /**
     * @brief Participant responsible for resolving issues at this level.
     */
    Participant* responsible;

    /**
     * @brief Maximum severity level this handler can resolve.
     */
    int maxSeverity;

public:

    /**
     * @brief Constructs an EscalationHandler.
     *
     * @param responsible Participant assigned to this handler.
     * @param maxSeverity Maximum severity the handler can resolve.
     */
    EscalationHandler(Participant* responsible, int maxSeverity);

    /**
     * @brief Virtual destructor.
     */
    virtual ~EscalationHandler();

    /**
     * @brief Sets the next handler in the escalation chain.
     *
     * @param handler Pointer to the next handler.
     */
    void setNext(EscalationHandler* handler);

    /**
     * @brief Attempts to handle an issue.
     *
     * If the issue severity is within the handler's responsibility,
     * the issue is resolved. Otherwise, it is passed to the next
     * handler in the chain.
     *
     * @param issue Issue to be processed.
     * @return true if the issue was handled successfully.
     * @return false if no suitable handler exists.
     */
    bool handleIssue(const Issue& issue);

    /**
     * @brief Resolves an issue.
     *
     * Must be implemented by derived classes.
     *
     * @param issue Issue to resolve.
     */
    virtual void resolve(const Issue& issue) = 0;
};

/**
 * @brief First-level support handler.
 *
 * Handles low-severity issues that can be resolved
 * at the frontline support level.
 */
class FrontlineHandler : public EscalationHandler {
public:

    /**
     * @brief Constructs a FrontlineHandler.
     *
     * @param responsible Assigned participant.
     * @param maxSeverity Maximum severity this handler can resolve.
     */
    FrontlineHandler(Participant* responsible, int maxSeverity);

    /**
     * @brief Resolves an issue at frontline level.
     *
     * @param issue Issue to resolve.
     */
    void resolve(const Issue& issue) override;
};

/**
 * @brief Second-level escalation handler.
 *
 * Handles medium-severity issues escalated from
 * frontline support.
 */
class SupervisorHandler : public EscalationHandler {
public:

    /**
     * @brief Constructs a SupervisorHandler.
     *
     * @param responsible Assigned participant.
     * @param maxSeverity Maximum severity this handler can resolve.
     */
    SupervisorHandler(Participant* responsible, int maxSeverity);

    /**
     * @brief Resolves an issue at supervisor level.
     *
     * @param issue Issue to resolve.
     */
    void resolve(const Issue& issue) override;
};

/**
 * @brief Final escalation handler.
 *
 * Handles high-severity issues requiring management
 * intervention.
 */
class ManagerHandler : public EscalationHandler {
public:

    /**
     * @brief Constructs a ManagerHandler.
     *
     * @param responsible Assigned participant.
     * @param maxSeverity Maximum severity this handler can resolve.
     */
    ManagerHandler(Participant* responsible, int maxSeverity);

    /**
     * @brief Resolves an issue at management level.
     *
     * @param issue Issue to resolve.
     */
    void resolve(const Issue& issue) override;
};

#endif