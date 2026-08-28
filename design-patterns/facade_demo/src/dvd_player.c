#include "dvd_player.h"
#include <stdio.h>
#include <string.h>

void dvd_player_init(DvdPlayer *d) {
    d->is_on = 0;
    d->is_playing = 0;
    strcpy(d->current_movie, "");
}

void dvd_player_on(DvdPlayer *d) {
    d->is_on = 1;
    printf("DVD Player: powering on\n");
}

void dvd_player_off(DvdPlayer *d) {
    d->is_on = 0;
    d->is_playing = 0;
    printf("DVD Player: powering off\n");
}

void dvd_player_play(DvdPlayer *d, const char *movie) {
    if (d->is_on) {
        d->is_playing = 1;
        strcpy(d->current_movie, movie);
        printf("DVD Player: playing '%s'\n", movie);
    }
}

void dvd_player_stop(DvdPlayer *d) {
    if (d->is_on && d->is_playing) {
        d->is_playing = 0;
        printf("DVD Player: stopped\n");
    }
}

void dvd_player_eject(DvdPlayer *d) {
    if (d->is_on && !d->is_playing) {
        printf("DVD Player: ejecting disc\n");
    }
}