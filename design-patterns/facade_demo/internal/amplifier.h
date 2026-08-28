#ifndef AMPLIFIER_H
#define AMPLIFIER_H

typedef struct Amplifier {
    int is_on;
    int volume;
    int surround_sound;
} Amplifier;

void amplifier_init(Amplifier *a);
void amplifier_on(Amplifier *a);
void amplifier_off(Amplifier *a);
void amplifier_set_volume(Amplifier *a, int volume);
void amplifier_set_surround_sound(Amplifier *a, int enable);

#endif
