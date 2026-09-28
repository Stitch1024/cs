#ifndef HOME_THEATER_FACADE_H
#define HOME_THEATER_FACADE_H

#include "projector.h"
#include "amplifier.h"
#include "dvd_player.h"

typedef struct
{
    Projector *projector; // Pointer to the projector created in client code
    Amplifier *amplifier; // Pointer to the amplifier created in client code
    DvdPlayer *dvd_player; // Pointer to the DVD player created in client code
} HomeTheaterFacade;

void home_theater_facade_init(HomeTheaterFacade *facade, Projector *projector, Amplifier *amplifier, DvdPlayer *dvd_player);
void home_theater_facade_watch_movie(HomeTheaterFacade *facade, const char *title);
void home_theater_facade_end_movie(HomeTheaterFacade *facade);
void home_theater_facade_listen_to_music(HomeTheaterFacade *facade, const char *album);
void home_theater_facade_stop_music(HomeTheaterFacade *facade);

#endif // HOME_THEATER_FACADE_H