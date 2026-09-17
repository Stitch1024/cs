#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

/* State - The condition or situation during the life of the system */
typedef enum {
    STATE_RED,      /* Red light: stop */
    STATE_GREEN,    /* Green light: go */
    STATE_YELLOW    /* Yellow light: warning, about to change */
} State;

/* Event - Something that happens and may trigger a state change */
typedef enum {
    EVENT_TIMEOUT,      /* Timer timeout: auto switch to next state */
    EVENT_BUTTON_PRESS, /* Button press: manual switch */
    EVENT_EMERGENCY     /* Emergency: immediately switch to red */
} Event;

/* Initialize the state machine */
void state_machine_init(void);

/* Handle an event */
void state_machine_handle_event(Event event);

/* Get current state */
State state_machine_get_current_state(void);

/* Get state name (for printing) */
const char* state_machine_get_state_name(State state);

/* Get event name (for printing) */
const char* state_machine_get_event_name(Event event);

#endif