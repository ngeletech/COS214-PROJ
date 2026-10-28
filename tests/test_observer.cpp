#include "doctest.h"

#include "Observer.h"
#include "WorkflowEvent.h"

TEST_CASE("Dashboard status is updated")
{
    DashboardListener dashboard;

    WorkflowEvent event(
        "TASK_COMPLETED",
        1,
        "PackGame",
        "Task finished"
    );

    dashboard.update(event);

    CHECK(
        dashboard.getStatus(1)
        == "TASK_COMPLETED"
    );
}

class MockListener : public WorkflowListener
{
public:
    bool updated = false;

    void update(const WorkflowEvent&) override
    {
        updated = true;
    }
};

TEST_CASE("Publisher notifies subscribed listeners")
{
    WorkflowEventPublisher publisher;

    MockListener listener;

    publisher.subscribe(&listener);

    WorkflowEvent event(
        "TASK_COMPLETED",
        1,
        "PackGame",
        "Task finished"
    );

    publisher.publish(event);

    CHECK(listener.updated);
}

TEST_CASE("Unknown status returns default value")
{
    DashboardListener dashboard;

    CHECK(
        dashboard.getStatus(999)
        == "Unknown"
    );
}