#include <stdio.h>
#include <string.h>
#include "uart_common.h"

/*===================== External Functions =====================*/

/* Bus functions */
extern void uart_bus_init(void);
extern void uart_bus_write(uint8_t bit);
extern uint8_t uart_bus_read(void);

/* TX FSM */
extern void uart_tx_fsm_init(UartTxFsm *fsm, uint32_t baud_rate);
extern void uart_tx_fsm_handle_event(UartTxFsm *fsm, UartEvent event, void (*send_bit)(uint8_t));
extern uint8_t uart_tx_fsm_is_idle(UartTxFsm *fsm);

/* RX FSM */
extern void uart_rx_fsm_init(UartRxFsm *fsm, uint32_t baud_rate);
extern void uart_rx_fsm_handle_event(UartRxFsm *fsm, UartEvent event, uint8_t rx_pin);
extern uint8_t uart_rx_fsm_is_data_ready(UartRxFsm *fsm);
extern uint8_t uart_rx_fsm_get_data(UartRxFsm *fsm);

/*===================== Delay Function =====================*/

#if defined(_WIN32) || defined(__linux__)
#include <unistd.h>
void delay_us(uint32_t us)
{
    usleep(us);
}
#else
void delay_us(uint32_t us)
{
    /* TODO: Implement with hardware timer */
}
#endif

/*===================== Send and Receive Byte =====================*/

/*
 * Send one byte and receive it immediately
 * - TX sends each bit to bus
 * - RX reads each bit from bus
 * - Simulates real-time communication
 */
void send_and_receive_byte(UartTxFsm *tx_fsm, UartRxFsm *rx_fsm, uint8_t byte)
{
    if (tx_fsm == NULL || rx_fsm == NULL)
    {
        printf("[ERR] send_and_receive_byte: NULL fsm pointer\n");
        return;
    }
    if (tx_fsm->baud_rate == 0 || rx_fsm->baud_rate == 0)
    {
        printf("[ERR] baud_rate is 0\n");
        return;
    }
    if (!uart_tx_fsm_is_idle(tx_fsm))
    {
        printf("[ERR] TX not idle (state=%d), byte 0x%02X dropped\n", tx_fsm->state, byte);
        return;
    }
    /* Clear stale data / recover from previous framing error for demo */
    if (uart_rx_fsm_is_data_ready(rx_fsm))
    {
        (void)uart_rx_fsm_get_data(rx_fsm);
    }
    if (rx_fsm->state == UART_STATE_ERROR)
    {
        uart_rx_fsm_handle_event(rx_fsm, UART_EVENT_TIMEOUT, 0);
    }

    uint32_t bit_time = UART_BIT_TIME_US(tx_fsm->baud_rate);

    printf("\n========== Sending 0x%02X ('%c') ==========\n",
           byte, (byte >= 32 && byte <= 126) ? byte : '?');

    /* Encapsulated: do NOT touch fsm->data/bit_count directly */
    uart_tx_fsm_set_data(tx_fsm, byte);

    /* Start transmission: IDLE -> START */
    uart_tx_fsm_handle_event(tx_fsm, UART_EVENT_TX_START, uart_bus_write);

    /* Send start bit (0) and let RX detect falling edge */
    uart_tx_fsm_handle_event(tx_fsm, UART_EVENT_TX_SEND_BIT, uart_bus_write);
    uart_rx_fsm_handle_event(rx_fsm, UART_EVENT_RX_START, uart_bus_read());

    /* Sample in the middle of start bit */
    delay_us(bit_time / 2);
    uart_rx_fsm_handle_event(rx_fsm, UART_EVENT_RX_BIT, uart_bus_read());
    delay_us(bit_time / 2);

    /* Send and receive data bits - middle sampling */
    for (int i = 0; i < UART_DATA_BITS; i++)
    {
        uart_tx_fsm_handle_event(tx_fsm, UART_EVENT_TX_SEND_BIT, uart_bus_write);
        delay_us(bit_time / 2);
        uart_rx_fsm_handle_event(rx_fsm, UART_EVENT_RX_BIT, uart_bus_read());
        delay_us(bit_time / 2);
    }

    /* Send and receive stop bit (1) - middle sampling */
    uart_tx_fsm_handle_event(tx_fsm, UART_EVENT_TX_SEND_BIT, uart_bus_write);
    delay_us(bit_time / 2);
    uart_rx_fsm_handle_event(rx_fsm, UART_EVENT_RX_STOP, uart_bus_read());
    delay_us(bit_time / 2);

    /* Check result - distinguish framing error vs no data */
    if (uart_rx_fsm_is_data_ready(rx_fsm))
    {
        uint8_t received = uart_rx_fsm_get_data(rx_fsm);
        printf("========== Received: 0x%02X ('%c') ==========\n",
               received, (received >= 32 && received <= 126) ? received : '?');
    }
    else if (rx_fsm->state == UART_STATE_ERROR)
    {
        printf("========== Framing Error (STOP bit != 1) ==========\n");
        /* Recover for next byte in demo */
        uart_rx_fsm_handle_event(rx_fsm, UART_EVENT_TIMEOUT, 0);
    }
    else
    {
        printf("========== Error: no data ready (state=%d) ==========\n", rx_fsm->state);
    }
}

/*===================== Main =====================*/

int main()
{
    UartTxFsm tx_fsm;
    UartRxFsm rx_fsm;

    printf("===== UART Linked Demo =====\n");
    printf("Sender and Receiver connected via UART bus\n\n");

    /* Initialize */
    uart_bus_init();
    uart_tx_fsm_init(&tx_fsm, UART_BAUD_RATE_9600);
    uart_rx_fsm_init(&rx_fsm, UART_BAUD_RATE_9600);

    /* Test 1: Send single bytes */
    printf("\n--- Test 1: Send 'H', 'i', '!' ---\n");
    send_and_receive_byte(&tx_fsm, &rx_fsm, 'H');
    send_and_receive_byte(&tx_fsm, &rx_fsm, 'i');
    send_and_receive_byte(&tx_fsm, &rx_fsm, '!');

    /* Test 2: Send string */
    printf("\n--- Test 2: Send \"Hello\" ---\n");
    const char *str = "Hello";
    while (*str)
    {
        send_and_receive_byte(&tx_fsm, &rx_fsm, *str++);
    }

    printf("\n===== Demo Complete =====\n");
    return 0;
}