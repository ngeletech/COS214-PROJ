cmake_minimum_required(VERSION 3.13)
project(TIVIDY CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include tests)

# ---------- engine sources: each pair adds their own files in their own block ----------
set(ENGINE_SRC
    # Builder
    src/WorkflowDesigner.cpp
    src/WorkflowDefinition.cpp
    src/StandardDefinitionBuilder.cpp

    # Factory Method
    src/WorkItemFactory.cpp
    src/WorkItemFactoryRegistry.cpp

    # Composite
    src/WorkElement.cpp
    src/Task.cpp
    src/WorkGroup.cpp
    src/ParallelGroup.cpp
    src/SequentialGroup.cpp

    # Iterator
    src/DepthFirstIterator.cpp
    src/AvailableItemIterator.cpp
    src/AuditEntry.cpp
    src/AuditEntryIterator.cpp
    src/AuditHistory.cpp
    src/ReportGenerator.cpp

    # State 
    src/WorkflowInstance.cpp
    src/InstanceState.cpp
    src/CreatedState.cpp
    src/RunningState.cpp
    src/SuspendedState.cpp
    src/CompletedState.cpp
    src/FailedState.cpp
    src/CancelledState.cpp

    # Memento
    src/InstanceSnapshot.cpp

    # Chain of Responsibility
    src/Issue.cpp
    src/Chain-Of-Responsibility.cpp
    src/FrontlineHandler.cpp
    src/SupervisorHandler.cpp
    src/ManagerHandler.cpp

    #Strategy 
    src/AssignmentService.cpp
    src/AttributeMatchingStrategy.cpp
    src/LeastLoadedStrategy.cpp
    src/RoleMatchingStrategy.cpp

    # Adapter 
    src/WorkData.cpp 
    src/RestApiAdapter.cpp
    src/RestApiClient.cpp
    src/LegacySystemAdapter.cpp
    src/LegacySystemApi.cpp
    src/MessagingAdapter.cpp
    src/MessageQueueClient.cpp
    src/ServiceRequest.cpp
    src/ServiceResponse.cpp

    # Decorator
    src/WorkElementDecorator.cpp
    src/PayloadDecorator.cpp
    src/AuditedDecorator.cpp
    src/PriorityDecorator.cpp

    # Template Method
    src/TemplateMethod.cpp

    # Observer
    src/WorkflowEvent.cpp
    src/WorkflowEventPublisher.cpp
    src/AuditLogger.cpp
    src/NotificationListener.cpp
    src/DashboardListener.cpp

    # Facade 
    src/TividyFacade.cpp

    # Command
    src/CommandInvoker.cpp
    src/WorkflowCommand.cpp
    src/StartInstanceCommand.cpp
    src/CancelCommand.cpp
    src/ChangeDataCommand.cpp
    src/NextItemActivator.cpp
    src/ReassignItemCommand.cpp
   
    src/Participant.cpp
    src/WorkflowEngine.cpp
)

if(ENGINE_SRC)
    add_library(tividy_engine ${ENGINE_SRC})
endif()

# ---------- tests: each pair adds their own test file in their own block ----------
add_executable(unit_tests
    tests/test_main.cpp
 
    # Builder + Factory Method
    tests/test_builder.cpp
    tests/test_factory_method.cpp

    # Composite + Iterator
    tests/test_composite.cpp   
    tests/test_iterator.cpp

    # State + Memento
    tests/test_state.cpp
    tests/test_memento.cpp

    # Strategy + Chain of Responsibility
    tests/test_strategy.cpp
    tests/test_chain_of_responsibility.cpp
    
    # Adapter + Decorator
    tests/test_adapter.cpp
    tests/test_decorator.cpp

    # Observer + Template Method
    tests/test_observer.cpp
    tests/test_template-method.cpp

    # Facade + Command
    tests/test_facade.cpp
    tests/test_command.cpp
)

if(TARGET tividy_engine)
    target_link_libraries(unit_tests tividy_engine)
endif()


# ---------- TUI application ----------
add_executable(tividy_app
    src/main.cpp
    src/TUI.cpp
    src/TerminalUI.cpp
    src/Scenarios.cpp
    src/WorkflowScreens.cpp
)

target_link_libraries(tividy_app tividy_engine)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
