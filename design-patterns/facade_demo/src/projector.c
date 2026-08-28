#include "projector.h"
#include <stdio.h>

void projector_init(Projector *p) {
    p->is_on = 0;
    p->input_source = 0;
}

void projector_on(Projector *p) {
    p->is_on = 1;
    printf("Projector: warming up...\n");
}

void projector_off(Projector *p) {
    p->is_on = 0;
    printf("Projector: cooling down...\n");
}

void projector_set_input(Projector *p, int source) {
    if (p->is_on) {
        p->input_source = source;
        printf("Projector: input set to %s\n", 
               source == 1 ? "DVD" : "HDMI");
    }
}

void projector_wide_screen_mode(Projector *p) {
    if (p->is_on) {
        printf("Projector: wide screen mode enabled\n");
    }
}