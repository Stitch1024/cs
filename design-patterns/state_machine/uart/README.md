# UART State Machine

A practical example of State Pattern applied to UART communication protocol. PC-side loopback simulation for learning FSM — no hardware required.

## 1. UART Protocol Overview

UART (Universal Asynchronous Receiver/Transmitter) is a serial communication protocol.

### UART Frame Format

```
IDLE(1) | START(0) | D0 | D1 | D2 | D3 | D4 | D5 | D6 | D7 | STOP(1) | IDLE(1)
──────────────────────────────────────────────────────────────────────────────────
   1        0       x    x    x    x    x    x    x    x       1         1
```

| Field | Description |
|-------|-------------|
| **IDLE** | Data line is high (1) |
| **START** | Data line goes low (0), indicates beginning of frame |
| **D0-D7** | 8 data bits, transmitted LSB first (`UART_DATA_BITS`) |
| **STOP** | Data line goes high (1), indicates end of frame |

## 2. State Machine Design

### States

| State | Description |
|-------|-------------|
| `IDLE` | Waiting for start bit (RX = 1) |
| `START` | Start bit detected (RX = 0) |
| `DATA` | Receiving/transmitting 8 data bits |
| `STOP` | Receiving/sending stop bit (RX = 1) |
| `ERROR` | Framing error, recovered by `TIMEOUT` |

### Events

Actual `uart_common.h` events (TX and RX share the state enum, events are split):

| Event | Description |
|-------|-------------|
| `UART_EVENT_TX_START` | TX: `IDLE -> START` |
| `UART_EVENT_TX_SEND_BIT` | TX: send start/data/stop bit |
| `UART_EVENT_RX_START` | RX: falling edge detected (`rx_pin==0`) |
| `UART_EVENT_RX_BIT` | RX: sample start/data bit |
| `UART_EVENT_RX_STOP` | RX: sample stop bit |
| `UART_EVENT_TIMEOUT` | Recover `ERROR -> IDLE` |

### State Transition Diagram

```
┌─────────┐  TX_START/RX_START  ┌─────────┐  TX_SEND/RX_BIT  ┌─────────┐  bit<8 ┌─────────┐
│  IDLE   │────────────────────►│  START  │─────────────────►│  DATA   │───────►│  STOP   │
└─────────┘                     └─────────┘                  └─────────┘        └─────────┘
    ▲                               │                            │                  │
    │                               │ false start                │ bit==8           │ RX_STOP
    │                               ▼                            ▼                  │
    │                          ┌─────────┐                  ┌─────────┐             │
    │                          │  ERROR  │◄─────────────────┤         │◄────────────┘
    │                          └─────────┘  framing error   └─────────┘
    │                               │ TIMEOUT
    └───────────────────────────────┘
```

### Transition Table

**TX FSM (`uart_tx.c`):**

| Current | Event | Next | Action |
|---------|-------|------|--------|
| IDLE | TX_START | START | `bit_count=0` |
| START | TX_SEND_BIT | DATA | `send_bit(0)` |
| DATA | TX_SEND_BIT | DATA/STOP | `send_bit(data>>bit_count&1)`, `bit_count++` |
| STOP | TX_SEND_BIT | IDLE | `send_bit(1)` |
| ERROR | TIMEOUT | IDLE | recover |

**RX FSM (`uart_rx.c`):**

| Current | Event | Next | Action |
|---------|-------|------|--------|
| IDLE | RX_START && rx==0 | START | `data=0, bit_count=0` |
| START | RX_BIT && rx==0 | DATA | valid start |
| START | RX_BIT && rx==1 | IDLE | false start |
| DATA | RX_BIT | DATA/STOP | `data \|= rx<<bit_count++` |
| STOP | RX_STOP && rx==1 | IDLE | `data_ready=1` |
| STOP | RX_STOP && rx==0 | ERROR | framing error |
| ERROR | TIMEOUT | IDLE | recover |

Data API is encapsulated — `uart_tx_fsm_set_data()` only works in `IDLE` (`uart_tx.c:36`), caller never touches `fsm->data/bit_count` directly.

## 3. Code Implementation

### Version 1: Switch-case (used in this project)

```c
typedef enum { IDLE, START, DATA, STOP, ERROR } State;
typedef enum { TX_START, TX_SEND_BIT, RX_START, RX_BIT, RX_STOP, TIMEOUT } Event;

void uart_tx_fsm_handle_event(UartTxFsm *fsm, Event e, void (*send_bit)(uint8_t)) {
    switch (fsm->state) {
        case IDLE:  if (e==TX_START) { fsm->state=START; fsm->bit_count=0; } break;
        case START: if (e==TX_SEND_BIT) { send_bit(0); fsm->state=DATA; } break;
        case DATA:  if (e==TX_SEND_BIT) { send_bit((fsm->data>>fsm->bit_count)&1); if(++fsm->bit_count>=UART_DATA_BITS) fsm->state=STOP; } break;
        case STOP:  if (e==TX_SEND_BIT) { send_bit(1); fsm->state=IDLE; } break;
        case ERROR: if (e==TIMEOUT) fsm->state=IDLE; break;
    }
}
```

### Version 2: Function Pointer Table (Advanced)

```c
typedef void (*StateHandler)(Event event);

void handle_idle(Event e)  { if (e==RX_START) { current_state=START; } }
void handle_start(Event e) { if (e==RX_BIT) current_state=(rx==0?DATA:IDLE); }
// ...

StateHandler handlers[] = {
    [IDLE]=handle_idle, [START]=handle_start, [DATA]=handle_data,
    [STOP]=handle_stop, [ERROR]=handle_error
};
void uart_handle_event(Event e) { handlers[current_state](e); }
```

## 4. Comparison: Two Approaches

| Aspect | Switch-case | Function Pointer Table |
|--------|-------------|------------------------|
| **Logic** | Same | Same |
| **Code size** | Smaller | Larger |
| **Readability** | Good for < 10 states | Good for 10+ states |
| **Extensibility** | Modify switch | Add new function |
| **Debugging** | Breakpoint in one place | Breakpoint per state |

### When to Use

| Scenario | Recommendation |
|----------|----------------|
| Small project (< 10 states) | Switch-case |
| Large project (10+ states) | Function pointer table |
| Quick prototype | Switch-case |
| Production code | Function pointer table |

**Logic is identical, only code organization differs.**

## 5. Implementation in This Project

### Files

| File | Description |
|------|-------------|
| `uart_common.h` | Shared types, `UART_BIT_TIME_US(baud)`, `UART_DATA_BITS`, TX/RX structs |
| `uart_tx.c` | TX FSM + `uart_tx_fsm_set_data()` |
| `uart_rx.c` | RX FSM, framing-error `ERROR` handling |
| `uart_bus.c` | Bus simulation (`uart_bus_write/read`, idle=1) |
| `main.c` | Linked loopback demo `send_and_receive_byte()` — middle sampling `t/2 + t/2` |
| `uart_sender.c` | Standalone TX demo |
| `uart_receiver.c` | Standalone RX demo (`receiver_simulate_receive`) |
| `CMakeLists.txt` | `uart_linked` / `uart_sender` / `uart_receiver` targets |

### Build and Run

```bash
# Single-file build does NOT work — must link all sources:
gcc -o uart_linked.exe main.c uart_tx.c uart_rx.c uart_bus.c
./uart_linked.exe

# Or via CMake (recommended)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
./build/uart_linked.exe
./build/uart_sender.exe
./build/uart_receiver.exe
```

### Sample Output

```
===== UART Linked Demo =====
Sender and Receiver connected via UART bus

[BUS] Initialized (idle=1)
[TX_FSM] Initialized, state=IDLE
[RX_FSM] Initialized, state=IDLE

--- Test 1: Send 'H', 'i', '!' ---

========== Sending 0x48 ('H') ==========
[TX_FSM] IDLE -> START
[TX_FSM] START -> DATA, sending start bit
[BUS] Write: 0
[BUS] Read: 0
[RX_FSM] IDLE -> START
[BUS] Read: 0
[RX_FSM] START -> DATA
[TX_FSM] DATA, sending bit 0: 0
[BUS] Write: 0
[BUS] Read: 0
[RX_FSM] DATA, received bit 0: 0 (data=0x00)
...
[TX_FSM] STOP -> IDLE, sending stop bit
[BUS] Write: 1
[RX_FSM] STOP -> IDLE, data ready: 0x48
========== Received: 0x48 ('H') ==========
```

Middle sampling (`delay(t/2) -> sample -> delay(t/2)`) is consistent across `main.c`, `uart_sender.c`, `uart_receiver.c`.

## 6. Embedded Applications

| Application | Use Case |
|-------------|----------|
| **GPIO UART** | Software UART when hardware UART unavailable |
| **Multi-UART** | Extra UART channels via software |
| **Protocol Parser** | Custom frame format (header, command, checksum) |
| **Low Power** | State recovery after wake-up |

## 7. Baud Rate Configuration

| Baud Rate | Bit Time | Use Case |
|-----------|----------|----------|
| 9600 | 104us | Low speed, long distance |
| 115200 | 8us | High speed, short distance |

```c
#define UART_BIT_TIME_US(baud) (1000000U / (baud))
#define UART_DATA_BITS         8
```

## References

- [State Pattern - Refactoring Guru](https://refactoring.guru/design-patterns/state)
- [UART Protocol](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter)
