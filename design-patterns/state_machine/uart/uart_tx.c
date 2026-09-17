#include <stdio.h>
#include "uart_common.h"

/*===================== TX State Machine =====================*/

/*
 * TX State Diagram:
 *
 * ┌───────┐  TX_START  ┌───────┐  TX_SEND  ┌───────┐  bit<8  ┌───────┐
 * │ IDLE  │───────────►│ START │──────────►│ DATA  │────────►│ STOP  │
 * └───────┘            └───────┘           └───────┘         └───────┘
 *     ▲                    │                    │                 │
 *     │                    │                    │ bit=8           │ TX_DONE
 *     │                    │                    ▼                 │
 *     │                    │               ┌───────┐              │
 *     │                    │               │(wait) │              │
 *     │                    │               └───────┘              │
 *     │                    │                    │                 │
 *     └────────────────────┴────────────────────┴─────────────────┘
 */

/* Initialize TX FSM */
void uart_tx_fsm_init(UartTxFsm *fsm, uint32_t baud_rate) {
    fsm->state = UART_STATE_IDLE;
    fsm->data = 0;
    fsm->bit_count = 0;
    fsm->baud_rate = baud_rate;
    printf("[TX_FSM] Initialized, state=IDLE\n");
}

/* Check if TX is idle */
uint8_t uart_tx_fsm_is_idle(UartTxFsm *fsm) {
    return (fsm->state == UART_STATE_IDLE);
}

/* Set data to send - encapsulated, only valid in IDLE */
void uart_tx_fsm_set_data(UartTxFsm *fsm, uint8_t data) {
    if (fsm == NULL) return;
    if (fsm->state == UART_STATE_IDLE) {
        fsm->data = data;
    }
}

/* Handle TX event */
void uart_tx_fsm_handle_event(UartTxFsm *fsm, UartEvent event, void (*send_bit)(uint8_t)) {
    switch (fsm->state) {
        
        case UART_STATE_IDLE:
            if (event == UART_EVENT_TX_START) {
                printf("[TX_FSM] IDLE -> START\n");
                fsm->state = UART_STATE_START;
                fsm->bit_count = 0;
            }
            break;
            
        case UART_STATE_START:
            if (event == UART_EVENT_TX_SEND_BIT) {
                /* Send start bit (low) */
                printf("[TX_FSM] START -> DATA, sending start bit\n");
                send_bit(0);
                fsm->state = UART_STATE_DATA;
            }
            break;
            
        case UART_STATE_DATA:
            if (event == UART_EVENT_TX_SEND_BIT) {
                /* Send data bit */
                uint8_t bit = (fsm->data >> fsm->bit_count) & 1;
                printf("[TX_FSM] DATA, sending bit %d: %d\n", fsm->bit_count, bit);
                send_bit(bit);
                fsm->bit_count++;
                
                if (fsm->bit_count >= UART_DATA_BITS) {
                    printf("[TX_FSM] DATA -> STOP\n");
                    fsm->state = UART_STATE_STOP;
                }
            }
            break;
            
        case UART_STATE_STOP:
            if (event == UART_EVENT_TX_SEND_BIT) {
                /* Send stop bit (high) */
                printf("[TX_FSM] STOP -> IDLE, sending stop bit\n");
                send_bit(1);
                fsm->state = UART_STATE_IDLE;
            }
            break;
            
        case UART_STATE_ERROR:
            if (event == UART_EVENT_TIMEOUT) {
                fsm->state = UART_STATE_IDLE;
            }
            break;
    }
}