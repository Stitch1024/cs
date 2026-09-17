# State Machine (状态机)

A collection of state machine examples demonstrating the State Pattern.

## Directory Structure

```
state_machine/
├── README.md           ← This file (State machine theory)
├── traffic_light/      ← Traffic light example
│   ├── CMakeLists.txt
│   ├── main.c
│   ├── state_machine.h
│   └── switch_case.c
└── uart/               ← UART receiver example
    ├── CMakeLists.txt
    ├── main.c
    └── README.md
```

## 1. What is a State Machine?

A state machine is a mathematical model of computation that can be in exactly one of a limited number of states at any given time.

### Finite State Machine (FSM)

We focus on **Finite State Machine (FSM)** - the most common type used in embedded systems.

| Type | Description | Use Case |
|------|-------------|----------|
| **Finite (FSM)** | Limited, fixed number of states | Embedded systems, protocols |
| **Infinite** | Unlimited states (theoretical) | Math theory, not practical |

**Why FSM?**
- States are **predictable** and **countable**
- Easy to implement and debug
- Fits embedded system constraints (limited memory/CPU)

### Core Elements

| Element | Description | Example |
|---------|-------------|---------|
| **State** | A condition or situation during the life of an object | IDLE, RUNNING, SLEEP |
| **Event** | Something that happens and may trigger a state change | Button press, Timer timeout |
| **Action** | Work performed during a state or transition | Turn on LED, Send data |
| **Transition** | Change from one state to another | IDLE → RUNNING |

## 2. Types of State Machines

### By State Count (Most Important for Embedded)

| Type | Description | Use Case |
|------|-------------|----------|
| **Finite State Machine (FSM)** | Fixed number of states | **Embedded systems (most common)** |
| **Infinite State Machine** | Unlimited states | Theoretical only |

### By Complexity

| Type | Description | Use Case |
|------|-------------|----------|
| **Moore** | Output depends only on current state | Simple LED control |
| **Mealy** | Output depends on current state AND input | Protocol handling |

### By Structure

| Type | Description | Use Case |
|------|-------------|----------|
| **Flat** | No nested states | Simple systems |
| **Hierarchical (HSM)** | States can contain sub-states | Complex systems (UML statecharts) |
| **Parallel** | Multiple state machines run concurrently | Multi-threaded systems |

### By Implementation

| Type | Description | Best For |
|------|-------------|----------|
| **Switch-case** | Simple if/else or switch | < 10 states |
| **Function pointer table** | Array of function pointers | 10+ states, complex logic |
| **State pattern (OOP)** | Each state is a class | Large systems, polymorphism |

## 3. Why Use State Machine?

### Benefits

| Benefit | Description |
|---------|-------------|
| **Predictable** | Same input + same state = same output |
| **Testable** | Easy to test each state independently |
| **Maintainable** | Adding new states doesn't affect existing logic |
| **Debuggable** | Easy to trace state transitions |
| **Decoupled** | Separates state logic from other code |

### Problems It Solves

| Problem | Without State Machine | With State Machine |
|---------|----------------------|-------------------|
| Complex conditional logic | Nested if/else hell | Clean state table |
| Hard to add new states | Modify scattered code | Add new state entry |
| Difficult debugging | Trace through complex flow | Log state transitions |
| Code duplication | Same checks everywhere | Centralized state logic |

## 4. How to Use State Machine

### Step 1: Identify States

Ask: "What are the distinct conditions my system can be in?"

```
Example: Traffic Light
- RED
- GREEN
- YELLOW
```

### Step 2: Identify Events

Ask: "What can trigger state changes?"

```
Example: Traffic Light
- Timer timeout
- Button press
- Emergency signal
```

### Step 3: Define Transitions

Ask: "From each state, what events can happen, and where do they go?"

```
RED    + timeout → GREEN
GREEN  + timeout → YELLOW
YELLOW + timeout → RED
```

### Step 4: Define Actions

Ask: "What should happen during each transition?"

```
Enter RED    → Turn on red LED
Enter GREEN  → Turn on green LED
Enter YELLOW → Turn on yellow LED
```

### Step 5: Implement

Choose implementation based on project size:
- **Switch-case**: Simple, good for < 10 states
- **Function pointer table**: More organized, good for 10+ states

## 5. Examples in This Project

| Example | Description | Location |
|---------|-------------|----------|
| **Traffic Light** | Simple state machine with 3 states | [traffic_light/](./traffic_light/) |
| **UART Receiver** | Real-world protocol state machine | [uart/](./uart/) |

### Build and Run

```bash
# Traffic light example
cd traffic_light
cmake -S . -B build
cmake --build build
./build/state_machine.exe

# UART example
cd uart
cmake -S . -B build
cmake --build build
./build/uart_state_machine.exe
```

## 6. Embedded Systems Examples

| Application | States | Events |
|-------------|--------|--------|
| **Button debounce** | IDLE, PRESSED, DEBOUNCE, RELEASED | Pin level change |
| **UART driver** | READY, BUSY, ERROR | Data received, TX complete |
| **Power management** | ACTIVE, IDLE, SLEEP, DEEP_SLEEP | Timeout, Wake-up signal |
| **Motor control** | STOPPED, STARTING, RUNNING, BRAKING | Speed command, Limit switch |

## References

- [State Design Pattern - Refactoring Guru](https://refactoring.guru/design-patterns/state)
- [UML Statecharts](https://en.wikipedia.org/wiki/UML_state_machine)
- [Game Programming Patterns - State](https://gameprogrammingpatterns.com/state.html)
