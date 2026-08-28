#ifndef DVD_PLAYER_H
#define DVD_PLAYER_H

typedef struct DvdPlayer {
    int is_on;
    int is_playing;
    char current_movie[100];
} DvdPlayer;

void dvd_player_init(DvdPlayer *d);
void dvd_player_on(DvdPlayer *d);
void dvd_player_off(DvdPlayer *d);
void dvd_player_play(DvdPlayer *d, const char *movie);
void dvd_player_stop(DvdPlayer *d);
void dvd_player_eject(DvdPlayer *d);

#endif
