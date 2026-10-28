cmake_minimum_required(VERSION 3.13)
project(TIVIDY CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include tests)

# ---------- engine sources: each pair adds their own files in their own block ----------
set(ENGINE_SRC
    # Builder + Factory Method
    src/WorkflowDesigner.cpp
    src/WorkflowDefinition.cpp
    src/StandardDefinitionBuilder.cpp

    # Composite + Iterator
    
    # State + Memento
    # src/WorkflowInstance.cpp
    src/InstanceState.cpp
    src/CreatedState.cpp
    src/RunningState.cpp
    src/SuspendedState.cpp
    src/CompletedState.cpp
    src/FailedState.cpp
    src/CancelledState.cpp

    # Strategy + Chain of Responsibility
    src/Participant.cpp

    # Adapter + Decorator
    src/WorkData.cpp                 
    src/RestApiAdapter.cpp           
    src/RestApiClient.cpp            
    src/LegacySystemAdapter.cpp      
    src/LegacySystemApi.cpp          
    src/MessagingAdapter.cpp         
    src/MessageQueueClient.cpp       
    src/ServiceRequest.cpp           
    src/ServiceResponse.cpp 
    src/WorkElementDecorator.cpp
    src/AuditedDecorator.cpp
    src/PriorityDecorator.cpp
    src/PayloadDecorator.cpp

    # Observer + Template Method
    src/Task.cpp
    src/TemplateMethod.cpp


    # Facade + Command
)

if(ENGINE_SRC)
    add_library(tividy_engine ${ENGINE_SRC})
endif()

# ---------- tests: each pair adds their own test file in their own block ----------
add_executable(unit_tests
    tests/test_main.cpp
    tests/test_smoke.cpp
    # Builder + Factory Method
    tests/test_builder.cpp

    # Composite + Iterator
    # tests/test_composite.cpp   
    # tests/test_iterator.cpp
    
    #ill uncomment this once workflowinstance builds

    # State + Memento
    # tests/test_state.cpp
    
    # Strategy + Chain of Responsibility
    src/Issue.cpp
    
    # Adapter + Decorator
    tests/test_adapter.cpp
    tests/test_decorator.cpp

    # Observer + Template Method

    # Facade + Command
)
if(TARGET tividy_engine)
    target_link_libraries(unit_tests tividy_engine)
endif()

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
