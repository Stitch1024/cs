#ifndef DVD_PLAYER_H
#define DVD_PLAYER_H

typedef struct
{
    int is_on;          // 0 = off, 1 = on
    int is_playing;     // 0 = stopped, 1 = playing
    char title[64];     // DVD title
} DvdPlayer;

void dvd_player_init(DvdPlayer *d);
void dvd_player_on(DvdPlayer *d);
void dvd_player_off(DvdPlayer *d);
void dvd_player_play(DvdPlayer *d, const char *title);
void dvd_player_stop(DvdPlayer *d);
void dvd_player_eject(DvdPlayer *d);

#endif // DVD_PLAYER_H