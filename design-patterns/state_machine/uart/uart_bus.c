#include <stdio.h>
#include "uart_common.h"

/*===================== UART Bus Simulation =====================*/

/*
 * UART Bus Simulation
 * - Simulates the physical wire between TX and RX
 * - TX writes to bus, RX reads from bus
 */

/* Bus state: 1 = idle (high), 0 = start bit (low) */
static uint8_t uart_bus_value = 1;

/* Write to bus (called by TX) */
void uart_bus_write(uint8_t bit) {
    uart_bus_value = bit;
    printf("[BUS] Write: %d\n", bit);
}

/* Read from bus (called by RX) */
uint8_t uart_bus_read(void) {
    uint8_t value = uart_bus_value;
    printf("[BUS] Read: %d\n", value);
    return value;
}

/* Initialize bus */
void uart_bus_init(void) {
    uart_bus_value = 1;  /* Idle high */
    printf("[BUS] Initialized (idle=1)\n");
}