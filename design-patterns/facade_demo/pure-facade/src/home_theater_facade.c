#include "home_theater_facade.h"
#include <stdio.h>

void home_theater_facade_init(HomeTheaterFacade *facade, Projector *projector, Amplifier *amplifier, DvdPlayer *dvd_player)
{
    facade->projector = projector;
    facade->amplifier = amplifier;
    facade->dvd_player = dvd_player;
}

void home_theater_facade_watch_movie(HomeTheaterFacade *facade, const char *title)
{
    // projector
    projector_on(facade->projector);
    projector_set_input(facade->projector, 1); // 1 = DVD player
    projector_wide_screen_mode(facade->projector);

    // amplifier
    amplifier_on(facade->amplifier);
    amplifier_set_volume(facade->amplifier, 5); // 5 = medium volume
    amplifier_set_surround_sound(facade->amplifier, 1); // 1 = on

    // dvd player
    dvd_player_on(facade->dvd_player);
    dvd_player_play(facade->dvd_player, title);
}

void home_theater_facade_end_movie(HomeTheaterFacade *facade)
{
    // dvd player
    dvd_player_stop(facade->dvd_player);
    dvd_player_eject(facade->dvd_player);
    dvd_player_off(facade->dvd_player);    

    // amplifier
    amplifier_off(facade->amplifier);

    // projector
    projector_off(facade->projector);
}

void home_theater_facade_listen_to_music(HomeTheaterFacade *facade, const char *album)
{
    // amplifier
    amplifier_on(facade->amplifier);
    amplifier_set_volume(facade->amplifier, 6);
    amplifier_set_surround_sound(facade->amplifier, 0); // 0 = off

    // dvd player
    dvd_player_on(facade->dvd_player);
    dvd_player_play(facade->dvd_player, album);
}

void home_theater_facade_stop_music(HomeTheaterFacade *facade)
{
    // dvd player
    dvd_player_stop(facade->dvd_player);
    dvd_player_eject(facade->dvd_player);

    // amplifier
    amplifier_off(facade->amplifier);
}