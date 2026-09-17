#include <stdio.h>
#include "state_machine.h"

/* Current state (global variable) */
static State current_state = STATE_GREEN;

/* Initialize the state machine */
void state_machine_init(void) {
    current_state = STATE_GREEN;
    printf("[INIT] Traffic light: GREEN\n");
}

/* 
 * Handle event - Core state machine logic
 * 
 * Parameters:
 *   event - The triggered event
 * 
 * State transition rules:
 *   RED    + timeout      -> GREEN
 *   GREEN  + timeout      -> YELLOW
 *   YELLOW + timeout      -> RED
 *   RED    + button_press -> GREEN (manual)
 *   GREEN  + button_press -> RED    (manual)
 *   *      + emergency    -> RED    (emergency)
 */
void state_machine_handle_event(Event event) {
    /* Print current state and event */
    printf("[CURRENT] State: %s, Event: %s\n",
           state_machine_get_state_name(current_state),
           state_machine_get_event_name(event));
    
    /* Emergency event: switch to RED from any state */
    if (event == EVENT_EMERGENCY) {
        printf("[ACTION] EMERGENCY! Turn on RED LED\n");
        current_state = STATE_RED;
        return;
    }
    
    /* Handle event based on current state */
    switch (current_state) {
        /* RED state */
        case STATE_RED:
            if (event == EVENT_TIMEOUT) {
                /* Timer timeout: RED -> GREEN */
                printf("[TRANSITION] RED -> GREEN\n");
                printf("[ACTION] Turn on GREEN LED\n");
                current_state = STATE_GREEN;
            } else if (event == EVENT_BUTTON_PRESS) {
                /* Button press: RED -> GREEN (manual) */
                printf("[TRANSITION] RED -> GREEN (manual)\n");
                printf("[ACTION] Turn on GREEN LED\n");
                current_state = STATE_GREEN;
            }
            break;

        /* GREEN state */
        case STATE_GREEN:
            if (event == EVENT_TIMEOUT) {
                /* Timer timeout: GREEN -> YELLOW */
                printf("[TRANSITION] GREEN -> YELLOW\n");
                printf("[ACTION] Turn on YELLOW LED\n");
                current_state = STATE_YELLOW;
            } else if (event == EVENT_BUTTON_PRESS) {
                /* Button press: GREEN -> RED (manual) */
                printf("[TRANSITION] GREEN -> RED (manual)\n");
                printf("[ACTION] Turn on RED LED\n");
                current_state = STATE_RED;
            }
            break;

        /* YELLOW state */
        case STATE_YELLOW:
            if (event == EVENT_TIMEOUT) {
                /* Timer timeout: YELLOW -> RED */
                printf("[TRANSITION] YELLOW -> RED\n");
                printf("[ACTION] Turn on RED LED\n");
                current_state = STATE_RED;
            }
            /* Button press in YELLOW state is ignored */
            break;

        /* Unknown state */
        default:
            printf("[ERROR] Unknown state: %d\n", current_state);
            break;
    }
}

/* Get current state */
State state_machine_get_current_state(void) {
    return current_state;
}

/* Get state name (for printing) */
const char* state_machine_get_state_name(State state) {
    switch (state) {
        case STATE_RED:    return "RED";
        case STATE_GREEN:  return "GREEN";
        case STATE_YELLOW: return "YELLOW";
        default:           return "UNKNOWN";
    }
}

/* Get event name (for printing) */
const char* state_machine_get_event_name(Event event) {
    switch (event) {
        case EVENT_TIMEOUT:      return "TIMEOUT";
        case EVENT_BUTTON_PRESS: return "BUTTON_PRESS";
        case EVENT_EMERGENCY:    return "EMERGENCY";
        default:                 return "UNKNOWN";
    }
}