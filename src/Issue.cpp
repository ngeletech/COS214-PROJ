#include "Issue.h"

Issue::Issue(const std::string& type, const std::string& description,
             int severity, const std::string& elementName)
    : type(type),
      description(description),
      severity(severity),
      elementName(elementName)
{
}

std::string Issue::getType() const
{
    return type;
}

std::string Issue::getDescription() const
{
    return description;
}

int Issue::getSeverity() const
{
    return severity;
}

std::string Issue::getElementName() const
{
    return elementName;
}