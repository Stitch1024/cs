#include <stdio.h>
#include <string.h>
#include "uart_common.h"

/*===================== Receiver Application =====================*/

/*
 * Receiver Application Layer
 * - Uses RX FSM to receive data
 * - Simulates receiving from UART bus
 */

/* GPIO simulation */
static uint8_t rx_pin_value = 1;

/* Set RX pin (for simulation) */
void receiver_set_rx_pin(uint8_t value) {
    rx_pin_value = value;
}

/* Delay simulation */
#if defined(_WIN32) || defined(__linux__)
    #include <unistd.h>
    void receiver_delay_us(uint32_t us) {
        usleep(us);
    }
#else
    void receiver_delay_us(uint32_t us) {
        /* TODO: Implement with hardware timer */
    }
#endif

/* Receive byte using RX FSM */
uint8_t receiver_receive_byte(UartRxFsm *fsm) {
    uint32_t bit_time = UART_BIT_TIME_US(fsm->baud_rate);
    
    printf("\n[RECEIVER] Waiting for byte...\n");
    
    /* Wait for start bit */
    while (rx_pin_value == 1) {
        /* Poll RX pin */
    }
    printf("[RECEIVER] Start bit detected\n");
    
    /* Start bit detected */
    uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_START, rx_pin_value);
    receiver_delay_us(bit_time / 2);  /* Sample in middle */
    
    /* Receive start bit */
    uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_BIT, rx_pin_value);
    receiver_delay_us(bit_time);
    
    /* Receive data bits */
    for (int i = 0; i < UART_DATA_BITS; i++) {
        uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_BIT, rx_pin_value);
        receiver_delay_us(bit_time);
    }
    
    /* Receive stop bit */
    uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_STOP, rx_pin_value);
    receiver_delay_us(bit_time);
    
    if (uart_rx_fsm_is_data_ready(fsm)) {
        uint8_t data = uart_rx_fsm_get_data(fsm);
        printf("[RECEIVER] Received: 0x%02X ('%c')\n", data, (data >= 32 && data <= 126) ? data : '?');
        return data;
    }
    
    return 0;
}

/* Simulate receiving a byte (for demo) - middle sampling, consistent with main.c */
void receiver_simulate_receive(UartRxFsm *fsm, uint8_t byte) {
    if (fsm == NULL || fsm->baud_rate == 0) return;
    if (fsm->state == UART_STATE_ERROR) {
        uart_rx_fsm_handle_event(fsm, UART_EVENT_TIMEOUT, 0);
    }
    uint32_t bit_time = UART_BIT_TIME_US(fsm->baud_rate);
    
    printf("\n[RECEIVER] Simulating receive: 0x%02X\n", byte);
    
    /* Start bit: falling edge -> sample middle */
    receiver_set_rx_pin(0);
    uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_START, rx_pin_value);
    receiver_delay_us(bit_time / 2);
    uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_BIT, rx_pin_value);
    receiver_delay_us(bit_time / 2);
    
    /* Data bits - middle sampling */
    for (int i = 0; i < UART_DATA_BITS; i++) {
        uint8_t bit = (byte >> i) & 1;
        receiver_set_rx_pin(bit);
        /* Pin must be stable before sampling, delay half then sample */
        receiver_delay_us(bit_time / 2);
        uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_BIT, rx_pin_value);
        receiver_delay_us(bit_time / 2);
    }
    
    /* Stop bit - middle sampling */
    receiver_set_rx_pin(1);
    receiver_delay_us(bit_time / 2);
    uart_rx_fsm_handle_event(fsm, UART_EVENT_RX_STOP, rx_pin_value);
    receiver_delay_us(bit_time / 2);
}

/*===================== Main (Receiver) =====================*/

int main() {
    UartRxFsm rx_fsm;
    
    printf("===== UART Receiver Device =====\n\n");
    
    /* Initialize RX FSM */
    uart_rx_fsm_init(&rx_fsm, UART_BAUD_RATE_9600);
    
    /* Receive single bytes */
    printf("\n--- Test 1: Receive single bytes ---\n");
    receiver_simulate_receive(&rx_fsm, 'H');
    receiver_simulate_receive(&rx_fsm, 'i');
    receiver_simulate_receive(&rx_fsm, '!');
    
    /* Receive string */
    printf("\n--- Test 2: Receive string ---\n");
    const char *str = "Hello UART!";
    while (*str) {
        receiver_simulate_receive(&rx_fsm, *str++);
    }
    
    printf("\n===== Receiver Complete =====\n");
    return 0;
}