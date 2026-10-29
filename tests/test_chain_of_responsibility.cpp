#include "doctest.h"

#include "Chain-Of-Responsibility.h"
#include "Issue.h"

TEST_CASE("Issue handled by frontline handler")
{
    FrontlineHandler frontline(nullptr, 2);

    Issue issue(
        "MissingPiece",
        "Missing token",
        1,
        "PackGame"
    );

    CHECK(frontline.handleIssue(issue));
}

TEST_CASE("Issue escalates to supervisor")
{
    FrontlineHandler frontline(nullptr, 2);
    SupervisorHandler supervisor(nullptr, 5);

    frontline.setNext(&supervisor);

    Issue issue(
        "DamagedGame",
        "Box damaged",
        4,
        "Inspection"
    );

    CHECK(frontline.handleIssue(issue));
}

TEST_CASE("Issue cannot be handled")
{
    FrontlineHandler frontline(nullptr, 2);
    SupervisorHandler supervisor(nullptr, 5);
    ManagerHandler manager(nullptr, 10);

    frontline.setNext(&supervisor);
    supervisor.setNext(&manager);

    Issue issue(
        "CriticalIssue",
        "Severity too high",
        99,
        "System"
    );

    CHECK_FALSE(frontline.handleIssue(issue));
}