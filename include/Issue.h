#ifndef ISSUE_H
#define ISSUE_H

#include <iostream>
#include <string>

/**
 * @brief Represents an issue that may be escalated through the
 * Chain of Responsibility.
 *
 * An Issue stores information about a workflow problem,
 * including its type, severity, description, and the workflow
 * element that generated it.
 */
class Issue
{
private:

    /**
     * @brief Category of the issue.
     */
    std::string type;

    /**
     * @brief Detailed description of the issue.
     */
    std::string description;

    /**
     * @brief Severity level of the issue.
     */
    int severity;

    /**
     * @brief Name of the workflow element associated with the issue.
     */
    std::string elementName;

public:

    /**
     * @brief Constructs a new Issue object.
     *
     * @param type Issue category.
     * @param description Description of the problem.
     * @param severity Severity level of the issue.
     * @param elementName Workflow element that produced the issue.
     */
    Issue(const std::string& type,
          const std::string& description,
          int severity,
          const std::string& elementName);

    /**
     * @brief Retrieves the issue type.
     *
     * @return The issue type.
     */
    std::string getType() const;

    /**
     * @brief Retrieves the issue description.
     *
     * @return The issue description.
     */
    std::string getDescription() const;

    /**
     * @brief Retrieves the severity level.
     *
     * @return Severity value.
     */
    int getSeverity() const;

    /**
     * @brief Retrieves the associated workflow element name.
     *
     * @return Workflow element name.
     */
    std::string getElementName() const;
};

#endif