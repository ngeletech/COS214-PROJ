# TIVIDY: a generic workflow management system

![CI](https://github.com/ngeletech/COS214-PROJ/actions/workflows/ci.yml/badge.svg)

TIVIDY is a reusable workflow engine written in C++11. A workflow **definition** describes the steps of a process, who may do them and what happens when something goes wrong. The engine runs many independent **instances** of a definition, each with its own data, state, participants and history.

The engine knows nothing about any industry. Two industries run on the same engine, started from the same menu, with no engine class changed:

| Industry | Workflow | Example outcome |
|---|---|---|
| Food service | `MealOrderDelivery` | An order is prepared, signed off by a manager and handed to a courier service |
| Telecommunications | `FiberProvisioning` | A fibre line request is processed through its provisioning steps |

Team Obsidian, COS 214 Final Project 2026, University of Pretoria.

---

## Quick start (Linux or WSL)

### Requirements
| Tool | Version |
|---|---|
| g++ or clang++ | C++11 support (g++ 7+ recommended) |
| CMake | 3.13 or newer |
| make | any |

The test framework (doctest) is included in `tests/doctest.h`, so nothing else needs installing.

```bash
sudo apt update && sudo apt install -y build-essential cmake
```

### Build
```bash
git clone https://github.com/ngeletech/COS214-PROJ.git
cd COS214-PROJ
cmake -S . -B build
cmake --build build
```

### Run the application
Run it from the project root, in a normal interactive terminal:
```bash
./build/tividy_app
```

### Run the tests
```bash
./build/unit_tests                       # prints every failed assertion
cd build && ctest --output-on-failure    # same suite through CTest
```
Expected result: `Status: SUCCESS!` with no failed test cases.

---

## Docker (no compiler or CMake needed)

The Dockerfile builds the project on Ubuntu 22.04, so the build is identical on every machine.

```bash
docker compose build
docker compose run --rm tui        # starts the interactive application
docker compose run --rm tests      # runs the unit tests
```

---

## Using the application

Navigate with the **UP / DOWN arrow keys** and press **ENTER** to select.

```
MAIN MENU
  1. Start Workflow        pick MealOrderDelivery or FiberProvisioning, enter its input data
  2. Create Workflow
  3. Active Workflows      list every instance and open one
  3. Exit

INSTANCE OVERVIEW           state, data, every work item with its status and assignee, history
  1. View Available Work / Interact
  2. Report Issue
  3. View Execution History
  4. Undo Last Command
  5. Refresh Overview
  6. Back
```

Inside **Available Work**, choosing an item shows the actions that make sense for it:

| Item type | Actions |
|---|---|
| Ordinary task (a person does it) | Complete Task, Reassign Task, Report Issue |
| Approval task | Approve, Reject, Reassign Task, Report Issue |

Script and service tasks run themselves when they become available.

### Demo path for either industry
1. **Start Workflow**, choose the industry and enter the input data it asks for.
2. In the overview, check the first work item, who it was assigned to, and the history.
3. **View Available Work / Interact**, select the item and **Complete Task**. The next step becomes available.
4. At the approval step, choose **Approve** to follow the **completion path** to `Completed`.
5. Start a second instance and choose **Reject** (or **Report Issue**) to follow the **exception path**. The issue goes through the escalation chain: it is either resolved and the step is offered again for rework, or the instance moves to `Failed`.
6. **View Execution History** to see the recorded events, and **Reassign Task** followed by **Undo Last Command** to see a command rolled back.

While the application runs, lines such as `[Task]: Executing task ...` and `[ApprovalTask]: ...` are printed by the engine. They are a deliberate trace of the Template Method steps, the issue handling and the external service calls.

---

## Design

### Generic engine, industry-specific configuration
Everything in the left column is reused by both industries. Everything in the right column lives in the scenario setup code (`src/Scenarios.cpp`).

| Generic engine (never changed per industry) | Supplied by each industry |
|---|---|
| `WorkflowEngine`, `WorkflowInstance`, `InstanceState` and its six states | Work item names and types |
| `WorkElement`, `Task`, `WorkGroup`, the task and group factories | Participants, their roles and attributes |
| `AssignmentService` and the assignment strategies | Which assignment rule the definition uses |
| `EscalationHandler` chain classes | How many escalation levels there are |
| `WorkflowEventPublisher` and the listeners | Which listeners are attached |
| `ExternalServiceConnector` and the adapters | Which external system a service step calls |
| `RollbackManager`, `CommandInvoker`, `TividyFacade` | Input data prompted at the start |

### The 14 Gang of Four patterns

| Category | Pattern | Where it is used |
|---|---|---|
| Creational | Builder | `WorkflowDesigner` drives `StandardDefinitionBuilder` to produce a validated, versioned `WorkflowDefinition` |
| Creational | Factory Method | `WorkItemFactory` and its concrete factories create each instance's own work elements, found through `WorkItemFactoryRegistry` |
| Structural | Composite | `WorkElement`, `Task`, `WorkGroup`, `ParallelGroup`, `SequentialGroup`: a step and a group are treated the same way |
| Structural | Decorator | `PayloadDecorator`, `AuditedDecorator`, `PriorityDecorator` wrap a work element |
| Structural | Adapter | `RestApiAdapter`, `LegacySystemAdapter`, `MessagingAdapter` give three different external systems one `ExternalServiceConnector` interface |
| Structural | Facade | `TividyFacade` is the only class the application talks to |
| Behavioural | State | `InstanceState` with `Created`, `Running`, `Suspended`, `Completed`, `Cancelled` and `Failed` states |
| Behavioural | Strategy | `AssignmentStrategy`: `RoleMatchingStrategy`, `AttributeMatchingStrategy`, `LeastLoadedStrategy` |
| Behavioural | Chain of Responsibility | `FrontlineHandler`, `SupervisorHandler`, `ManagerHandler` escalate an issue until someone resolves it |
| Behavioural | Observer | `WorkflowEventPublisher` notifies `AuditLogger`, `NextItemActivator`, `NotificationListener`, `DashboardListener` |
| Behavioural | Memento | `InstanceSnapshot` and `RollbackManager` save and restore an instance |
| Behavioural | Template Method | `Task` fixes the steps of execution, and `UserTask`, `ServiceTask`, `ApprovalTask`, `ScriptTask` supply the work step |
| Behavioural | Command | `WorkflowCommand` with `StartInstanceCommand`, `CancelInstanceCommand`, `ChangeDataCommand`, `ReassignItemCommand`, run by `CommandInvoker` |
| Behavioural | Iterator | `DepthFirstIterator`, `AvailableItemIterator` walk the work tree, and `AuditEntryIterator` reads the history |

The full justification, participant mapping and UML for each pattern are in the report.

---

## Testing and continuous integration

- **Framework:** doctest, one test file per pattern in `tests/`, plus facade tests that run whole workflows end to end.
- **Covered behaviour:** lifecycle transitions, assignment and routing, approval and rejection, escalation, instance isolation, undo, and invalid operations.
- **CI:** `.github/workflows/ci.yml` builds with g++ and clang++ and runs the full suite on every push and pull request to `main` and `dev`.

## Documentation

| What | Where |
|---|---|
| Final report (PDF) | [`report/`](report/) |
| UML diagrams (exports and Visual Paradigm project) | [`docs/diagrams`](docs/diagrams) |
| Class diagram | [`docs/UML.pdf`](docs/UML.pdf) |
| Source code documentation (Doxygen) | generate with `doxygen Doxyfile`, then open `docs/html/index.html` or `wslview docs/html/index.html` |


## Team and workflow

Ngeletshedzo Mutwanamba (team leader), Amira Ajanaku, Nare Maela, Nontokozo Madela, Manqoba Mashego, Giselle Mpye and Shepherd Nzimande.

Work is done on branches named after the pattern or feature, merged into `dev` through reviewed pull requests, with `dev` merged into `main` when stable. CI must pass before a pull request is merged.

Thanks for checking out our project.
