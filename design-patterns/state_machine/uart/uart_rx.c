#include <stdio.h>
#include "uart_common.h"

/*===================== RX State Machine =====================*/

/*
 * RX State Diagram:
 *
 * ┌───────┐  RX_START  ┌───────┐  RX_BIT   ┌───────┐  bit<8  ┌───────┐
 * │ IDLE  │───────────►│ START │──────────►│ DATA  │────────►│ STOP  │
 * └───────┘            └───────┘           └───────┘         └───────┘
 *     ▲                    │                    │                 │
 *     │                    │                    │ bit=8           │ RX_STOP
 *     │                    │                    ▼                 │
 *     │                    │               ┌───────┐              │
 *     │                    │               │(wait) │              │
 *     │                    │               └───────┘              │
 *     │                    │                    │                 │
 *     └────────────────────┴────────────────────┴─────────────────┘
 */

/* Initialize RX FSM */
void uart_rx_fsm_init(UartRxFsm *fsm, uint32_t baud_rate) {
    fsm->state = UART_STATE_IDLE;
    fsm->data = 0;
    fsm->bit_count = 0;
    fsm->baud_rate = baud_rate;
    fsm->data_ready = 0;
    printf("[RX_FSM] Initialized, state=IDLE\n");
}

/* Check if data is ready */
uint8_t uart_rx_fsm_is_data_ready(UartRxFsm *fsm) {
    return fsm->data_ready;
}

/* Get received data */
uint8_t uart_rx_fsm_get_data(UartRxFsm *fsm) {
    fsm->data_ready = 0;
    return fsm->data;
}

/* Handle RX event */
void uart_rx_fsm_handle_event(UartRxFsm *fsm, UartEvent event, uint8_t rx_pin) {
    switch (fsm->state) {
        
        case UART_STATE_IDLE:
            if (event == UART_EVENT_RX_START && rx_pin == 0) {
                /* Start bit detected */
                printf("[RX_FSM] IDLE -> START\n");
                fsm->state = UART_STATE_START;
                fsm->data = 0;
                fsm->bit_count = 0;
            }
            break;
            
        case UART_STATE_START:
            if (event == UART_EVENT_RX_BIT) {
                /* Sample start bit */
                if (rx_pin == 0) {
                    printf("[RX_FSM] START -> DATA\n");
                    fsm->state = UART_STATE_DATA;
                } else {
                    printf("[RX_FSM] START -> IDLE (false start)\n");
                    fsm->state = UART_STATE_IDLE;
                }
            }
            break;
            
        case UART_STATE_DATA:
            if (event == UART_EVENT_RX_BIT) {
                /* Store data bit */
                fsm->data |= (rx_pin << fsm->bit_count);
                printf("[RX_FSM] DATA, received bit %d: %d (data=0x%02X)\n", 
                       fsm->bit_count, rx_pin, fsm->data);
                fsm->bit_count++;
                
                if (fsm->bit_count >= UART_DATA_BITS) {
                    printf("[RX_FSM] DATA -> STOP\n");
                    fsm->state = UART_STATE_STOP;
                }
            }
            break;
            
        case UART_STATE_STOP:
            if (event == UART_EVENT_RX_STOP) {
                /* Stop bit received */
                if (rx_pin == 1) {
                    printf("[RX_FSM] STOP -> IDLE, data ready: 0x%02X\n", fsm->data);
                    fsm->data_ready = 1;
                    fsm->state = UART_STATE_IDLE;
                } else {
                    printf("[RX_FSM] STOP -> ERROR (framing error)\n");
                    fsm->state = UART_STATE_ERROR;
                }
            }
            break;
            
        case UART_STATE_ERROR:
            if (event == UART_EVENT_TIMEOUT) {
                fsm->state = UART_STATE_IDLE;
            }
            break;
    }
}