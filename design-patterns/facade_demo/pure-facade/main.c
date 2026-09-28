#include "home_theater_facade.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    Projector projector;
    Amplifier amplifier;
    DvdPlayer dvd_player;

    HomeTheaterFacade facade;

    projector_init(&projector);
    amplifier_init(&amplifier);
    dvd_player_init(&dvd_player);

    home_theater_facade_init(&facade, &projector, &amplifier, &dvd_player);

    printf("\n--- Path A: through the facade ---\n");
    home_theater_facade_watch_movie(&facade, "Inception");
    home_theater_facade_end_movie(&facade);

    printf("\n--- Path B: direct calls to subsystems ---\n");
    // Watch movie
    // projector
    projector_on(&projector);
    projector_set_input(&projector, 1); // 1 = DVD player
    projector_wide_screen_mode(&projector);
    // amplifier
    amplifier_on(&amplifier);
    amplifier_set_volume(&amplifier, 5); // 5 = medium volume
    amplifier_set_surround_sound(&amplifier, 1); // 1 = on
    // dvd player
    dvd_player_on(&dvd_player);
    dvd_player_play(&dvd_player, "dvd title");

    // End movie
    // dvd player
    dvd_player_stop(&dvd_player);
    dvd_player_eject(&dvd_player);
    dvd_player_off(&dvd_player);    
    // amplifier
    amplifier_off(&amplifier);
    // projector
    projector_off(&projector);

    return 0;
}