#include "dvd_player.h"
#include <stdio.h>
#include <string.h>

void dvd_player_init(DvdPlayer *d)
{
    d->is_on = 0;
    d->is_playing = 0;
    // Method 1: set only the first byte to '\0'(faster, but bytes 1-63 keep stale data)
    // d->title[0] = '\0';
    // Method 2: zero out the whole 64-byte buffer(cleanest statem recommended for init)
    memset(d->title, 0, sizeof(d->title));
}

void dvd_player_on(DvdPlayer *d)
{
    d->is_on = 1;
    printf("DVD Player: on\n");
}

void dvd_player_off(DvdPlayer *d)
{
    d->is_on = 0;
    printf("DVD Player: off\n");
}

void dvd_player_play(DvdPlayer *d, const char *title)
{
    if(d->is_on)
    {
        d->is_playing = 1;

        // Method 1: manual copy, must reserve 1 byte for '\0' and add it by hand
        // strncpy(d->title, title, sizeof(d->title) - 1);
        // d->title[sizeof(d->title) - 1] = '\0';

        // Method 2: snprintf truncates and null-terminates automatically, so it's safer and cleaner
        snprintf(d->title, sizeof(d->title), "%s", title);
        printf("DVD Player: playing \"%s\"\n", d->title);
    }
}

void dvd_player_stop(DvdPlayer *d)
{
    if(d->is_on && d->is_playing)
    {
        d->is_playing = 0;
        printf("DVD Player: stopped \"%s\"\n", d->title);
    }
}

void dvd_player_eject(DvdPlayer *d)
{
    if(d->is_on)
    {
        d->is_playing = 0;
        memset(d->title, 0, sizeof(d->title));
        printf("DVD Player: ejected\n");
    }
}