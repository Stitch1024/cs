#include "amplifier.h"
#include <stdio.h>

void amplifier_init(Amplifier *a)
{
    a->is_on = 0;
    a->volume = 0;
    a->surround = 0;
}

void amplifier_on(Amplifier *a)
{
    a->is_on = 1;
    printf("Amplifier: on\n");
}

void amplifier_off(Amplifier *a)
{
    a->is_on = 0;
    printf("Amplifier: off\n");
}

void amplifier_set_volume(Amplifier *a, int volume)
{
    if(a->is_on)
    {
        a->volume = volume;
        printf("Amplifier: volume = %d\n", volume);
    }
}

void amplifier_set_surround_sound(Amplifier *a, int surround)
{
    if(a->is_on)
    {
        a->surround = surround;
        printf("Amplifier: surround sound = %s\n", surround ? "on" : "off");
    }
}

