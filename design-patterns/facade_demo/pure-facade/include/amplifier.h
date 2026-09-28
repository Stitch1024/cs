#ifndef AMPLIFIER_H
#define AMPLIFIER_H

typedef struct
{
    int is_on;          // 0 = off, 1 = on
    int volume;         // 0 - 100
    int surround;       // 0 = off, 1 = on
} Amplifier;

void amplifier_init(Amplifier *a);
void amplifier_on(Amplifier *a);
void amplifier_off(Amplifier *a);
void amplifier_set_volume(Amplifier *a, int volume);
void amplifier_set_surround_sound(Amplifier *a, int surround);

#endif // AMPLIFIER_H