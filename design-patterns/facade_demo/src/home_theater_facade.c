#include "home_theater_facade.h"
#include "projector.h"
#include "amplifier.h"
#include "dvd_player.h"
#include <stdio.h>
#include <stdlib.h>

HomeTheaterFacade* home_theater_facade_create(void) {
    HomeTheaterFacade *facade = (HomeTheaterFacade*)malloc(sizeof(HomeTheaterFacade));
    if (facade) {
        facade->projector = (Projector*)malloc(sizeof(Projector));
        facade->amplifier = (Amplifier*)malloc(sizeof(Amplifier));
        facade->dvd_player = (DvdPlayer*)malloc(sizeof(DvdPlayer));
        
        if (facade->projector && facade->amplifier && facade->dvd_player) {
            projector_init(facade->projector);
            amplifier_init(facade->amplifier);
            dvd_player_init(facade->dvd_player);
        }
    }
    return facade;
}

void home_theater_facade_destroy(HomeTheaterFacade *facade) {
    if (facade) {
        free(facade->projector);
        free(facade->amplifier);
        free(facade->dvd_player);
        free(facade);
    }
}

void home_theater_facade_watch_movie(HomeTheaterFacade *facade, const char *movie) {
    printf("=== Get ready to watch a movie... ===\n");
    
    // 1. Turn on projector
    projector_on(facade->projector);
    
    // 2. Turn on amplifier
    amplifier_on(facade->amplifier);
    amplifier_set_volume(facade->amplifier, 5);
    amplifier_set_surround_sound(facade->amplifier, 1);
    
    // 3. Set projector input source
    projector_set_input(facade->projector, 1);  // DVD
    projector_wide_screen_mode(facade->projector);
    
    // 4. Turn on DVD player
    dvd_player_on(facade->dvd_player);
    dvd_player_play(facade->dvd_player, movie);
    
    printf("=== Enjoy the movie! ===\n\n");
}

void home_theater_facade_end_movie(HomeTheaterFacade *facade) {
    printf("=== Shutting movie theater down... ===\n");
    
    dvd_player_stop(facade->dvd_player);
    dvd_player_eject(facade->dvd_player);
    
    amplifier_off(facade->amplifier);
    
    projector_off(facade->projector);
    
    printf("=== Goodbye! ===\n\n");
}

void home_theater_facade_listen_to_music(HomeTheaterFacade *facade, const char *album) {
    printf("=== Get ready to listen to music... ===\n");
    
    // 1. Turn on amplifier
    amplifier_on(facade->amplifier);
    amplifier_set_volume(facade->amplifier, 6);
    amplifier_set_surround_sound(facade->amplifier, 0);
    
    // 2. Turn on DVD player
    dvd_player_on(facade->dvd_player);
    dvd_player_play(facade->dvd_player, album);
    
    printf("=== Enjoy the music! ===\n\n");
}

void home_theater_facade_stop_music(HomeTheaterFacade *facade) {
    printf("=== Stopping music... ===\n");
    
    dvd_player_stop(facade->dvd_player);
    amplifier_off(facade->amplifier);
    
    printf("=== Music stopped! ===\n\n");
}
