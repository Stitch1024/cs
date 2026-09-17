#ifndef UART_COMMON_H
#define UART_COMMON_H

#include <stdint.h>

/*===================== Baud Rate Configuration =====================*/

#define UART_BAUD_RATE_9600      9600
#define UART_BAUD_RATE_115200    115200
#define UART_BIT_TIME_US(baud)   (1000000U / (baud))
#define UART_DATA_BITS           8
#define UART_STOP_BITS           1

/*===================== States =====================*/

typedef enum {
    UART_STATE_IDLE,
    UART_STATE_START,
    UART_STATE_DATA,
    UART_STATE_STOP,
    UART_STATE_ERROR
} UartState;

/*===================== Events =====================*/

typedef enum {
    UART_EVENT_TX_START,        /* Start transmission */
    UART_EVENT_TX_SEND_BIT,     /* Send next bit */
    UART_EVENT_RX_START,        /* Start bit detected */
    UART_EVENT_RX_BIT,          /* Receive bit */
    UART_EVENT_RX_STOP,         /* Stop bit received */
    UART_EVENT_TIMEOUT          /* Recover from ERROR -> IDLE */
} UartEvent;

/*===================== TX FSM =====================*/

typedef struct {
    UartState state;
    uint8_t data;
    uint8_t bit_count;
    uint32_t baud_rate;
} UartTxFsm;

void uart_tx_fsm_init(UartTxFsm *fsm, uint32_t baud_rate);
void uart_tx_fsm_handle_event(UartTxFsm *fsm, UartEvent event, void (*send_bit)(uint8_t));
uint8_t uart_tx_fsm_is_idle(UartTxFsm *fsm);
void uart_tx_fsm_set_data(UartTxFsm *fsm, uint8_t data);

/*===================== RX FSM =====================*/

typedef struct {
    UartState state;
    uint8_t data;
    uint8_t bit_count;
    uint32_t baud_rate;
    uint8_t data_ready;
} UartRxFsm;

void uart_rx_fsm_init(UartRxFsm *fsm, uint32_t baud_rate);
void uart_rx_fsm_handle_event(UartRxFsm *fsm, UartEvent event, uint8_t rx_pin);
uint8_t uart_rx_fsm_is_data_ready(UartRxFsm *fsm);
uint8_t uart_rx_fsm_get_data(UartRxFsm *fsm);

#endif