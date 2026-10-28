#include "doctest.h"
#include "State.h"
#include <type_traits>

TEST_CASE("State classes derive from InstanceState") {
    CHECK(std::is_base_of<InstanceState, CreatedState>::value);
    CHECK(std::is_base_of<InstanceState, RunningState>::value);
    CHECK(std::is_base_of<InstanceState, SuspendedState>::value);
    CHECK(std::is_base_of<InstanceState, CompletedState>::value);
    CHECK(std::is_base_of<InstanceState, FailedState>::value);
    CHECK(std::is_base_of<InstanceState, CancelledState>::value);
}