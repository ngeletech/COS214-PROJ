cmake_minimum_required(VERSION 3.13)
project(TIVIDY CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include tests)

# ---------- engine sources: each pair adds their own files in their own block ----------
set(ENGINE_SRC
    # Builder + Factory Method

    # Composite + Iterator

    # State + Memento

    # Strategy + Chain of Responsibility

    # Adapter + Decorator

    # Observer + Template Method
    src/WorkflowEvent.cpp
    src/WorkflowEventPublisher.cpp
    src/AuditLogger.cpp
    src/NotificationListener.cpp
    src/DashboardListener.cpp

    # Facade + Command
)

if(ENGINE_SRC)
    add_library(tividy_engine ${ENGINE_SRC})
endif()

# ---------- tests: each pair adds their own test file in their own block ----------
add_executable(unit_tests
    tests/test_main.cpp
    tests/test_smoke.cpp
    tests/test_observer.cpp
    # Builder + Factory Method

    # Composite + Iterator

    # State + Memento

    # Strategy + Chain of Responsibility

    # Adapter + Decorator

    # Observer + Template Method

    # Facade + Command
)
if(TARGET tividy_engine)
    target_link_libraries(unit_tests tividy_engine)
endif()

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
