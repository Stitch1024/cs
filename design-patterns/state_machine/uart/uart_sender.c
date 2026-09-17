#include <stdio.h>
#include <string.h>
#include "uart_common.h"

/*===================== Sender Application =====================*/

/*
 * Sender Application Layer
 * - Uses TX FSM to send data
 * - Simulates sending to UART bus
 */

/* GPIO simulation */
static uint8_t tx_pin_value = 1;

/* Send bit callback */
void sender_send_bit(uint8_t bit) {
    tx_pin_value = bit;
    printf("[GPIO] TX = %d\n", bit);
}

/* Delay simulation */
#if defined(_WIN32) || defined(__linux__)
    #include <unistd.h>
    void sender_delay_us(uint32_t us) {
        usleep(us);
    }
#else
    void sender_delay_us(uint32_t us) {
        /* TODO: Implement with hardware timer */
    }
#endif

/* Send byte using TX FSM */
void sender_send_byte(UartTxFsm *fsm, uint8_t byte) {
    if (fsm == NULL || fsm->baud_rate == 0 || !uart_tx_fsm_is_idle(fsm)) return;
    uint32_t bit_time = UART_BIT_TIME_US(fsm->baud_rate);
    
    printf("\n[SENDER] Sending byte: 0x%02X ('%c')\n", byte, (byte >= 32 && byte <= 126) ? byte : '?');
    
    /* Encapsulated: use setter, bit_count reset is inside TX_START */
    uart_tx_fsm_set_data(fsm, byte);
    
    /* Start transmission: IDLE -> START (no bus activity, no delay) */
    uart_tx_fsm_handle_event(fsm, UART_EVENT_TX_START, sender_send_bit);
    
    /* Send start bit (0) - holds for 1 bit_time */
    uart_tx_fsm_handle_event(fsm, UART_EVENT_TX_SEND_BIT, sender_send_bit);
    sender_delay_us(bit_time);
    
    /* Send data bits */
    for (int i = 0; i < UART_DATA_BITS; i++) {
        uart_tx_fsm_handle_event(fsm, UART_EVENT_TX_SEND_BIT, sender_send_bit);
        sender_delay_us(bit_time);
    }
    
    /* Send stop bit (1) */
    uart_tx_fsm_handle_event(fsm, UART_EVENT_TX_SEND_BIT, sender_send_bit);
    sender_delay_us(bit_time);
    
    printf("[SENDER] Byte sent complete\n");
}

/* Send string */
void sender_send_string(UartTxFsm *fsm, const char *str) {
    printf("\n[SENDER] Sending string: \"%s\"\n", str);
    
    while (*str) {
        sender_send_byte(fsm, *str++);
    }
    
    printf("[SENDER] String sent complete\n");
}

/*===================== Main (Sender) =====================*/

int main() {
    UartTxFsm tx_fsm;
    
    printf("===== UART Sender Device =====\n\n");
    
    /* Initialize TX FSM */
    uart_tx_fsm_init(&tx_fsm, UART_BAUD_RATE_9600);
    
    /* Send single bytes */
    printf("\n--- Test 1: Send single bytes ---\n");
    sender_send_byte(&tx_fsm, 'H');
    sender_send_byte(&tx_fsm, 'i');
    sender_send_byte(&tx_fsm, '!');
    
    /* Send string */
    printf("\n--- Test 2: Send string ---\n");
    sender_send_string(&tx_fsm, "Hello UART!");
    
    printf("\n===== Sender Complete =====\n");
    return 0;
}