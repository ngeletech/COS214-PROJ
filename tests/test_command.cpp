#include "doctest.h"
#include "WorkflowCommand.h"
#include "State.h"
#include "WorkData.h"

TEST_CASE("ChangeDataCommand updates workflow instance data") {
    WorkflowInstance instance(1, nullptr, WorkData(), nullptr);

    ChangeDataCommand command(&instance, "status", "approved");
    command.execute();

    CHECK(instance.getData().getData().at("status") == "approved");
}
