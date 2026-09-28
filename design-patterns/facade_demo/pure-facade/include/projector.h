#ifndef PROJECTOR_H
#define PROJECTOR_H

typedef struct
{
    int is_on;          // 0 = off, 1 = on
    int input_source;   // 1 = DVD, 2 = HDMI
} Projector;

void projector_init(Projector *p);
void projector_on(Projector *p);
void projector_off(Projector *p);
void projector_set_input(Projector *p, int source);
void projector_wide_screen_mode(Projector *p);

#endif // PROJECTOR_H