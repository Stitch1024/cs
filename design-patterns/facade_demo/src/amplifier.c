#include "amplifier.h"
#include <stdio.h>

void amplifier_init(Amplifier *a) {
    a->is_on = 0;
    a->volume = 0;
    a->surround_sound = 0;
}

void amplifier_on(Amplifier *a) {
    a->is_on = 1;
    printf("Amplifier: powering on\n");
}

void amplifier_off(Amplifier *a) {
    a->is_on = 0;
    printf("Amplifier: powering off\n");
}

void amplifier_set_volume(Amplifier *a, int volume) {
    if (a->is_on) {
        a->volume = volume;
        printf("Amplifier: volume set to %d\n", volume);
    }
}

void amplifier_set_surround_sound(Amplifier *a, int enable) {
    if (a->is_on) {
        a->surround_sound = enable;
        printf("Amplifier: surround sound %s\n", 
               enable ? "enabled" : "disabled");
    }
}