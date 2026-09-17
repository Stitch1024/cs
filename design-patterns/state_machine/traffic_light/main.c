#include <stdio.h>
#include "state_machine.h"

int main() {
    printf("===== State Machine Demo (Switch-case) =====\n\n");
    
    /* Initialize state machine */
    state_machine_init();
    
    /* Test 1: Normal timeout transitions */
    printf("\n--- Test 1: Normal timeout transitions ---\n");
    for (int i = 0; i < 3; i++) {
        printf("\n[EVENT] Timer timeout\n");
        state_machine_handle_event(EVENT_TIMEOUT);
    }
    
    /* Test 2: Manual button press */
    printf("\n--- Test 2: Manual button press ---\n");
    printf("\n[EVENT] Button press\n");
    state_machine_handle_event(EVENT_BUTTON_PRESS);
    
    printf("\n[EVENT] Button press\n");
    state_machine_handle_event(EVENT_BUTTON_PRESS);
    
    /* Test 3: Emergency */
    printf("\n--- Test 3: Emergency ---\n");
    printf("\n[EVENT] Emergency\n");
    state_machine_handle_event(EVENT_EMERGENCY);
    
    printf("\n[EVENT] Button press (should stay RED)\n");
    state_machine_handle_event(EVENT_BUTTON_PRESS);
    
    printf("\n===== End =====\n");
    return 0;
}