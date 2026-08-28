#ifndef HOME_THEATER_FACADE_H
#define HOME_THEATER_FACADE_H

typedef struct Projector Projector;
typedef struct Amplifier Amplifier;
typedef struct DvdPlayer DvdPlayer;

typedef struct {
    Projector *projector;
    Amplifier *amplifier;
    DvdPlayer *dvd_player;
} HomeTheaterFacade;

HomeTheaterFacade* home_theater_facade_create(void);
void home_theater_facade_destroy(HomeTheaterFacade *facade);
void home_theater_facade_watch_movie(HomeTheaterFacade *facade, const char *movie);
void home_theater_facade_end_movie(HomeTheaterFacade *facade);
void home_theater_facade_listen_to_music(HomeTheaterFacade *facade, const char *album);
void home_theater_facade_stop_music(HomeTheaterFacade *facade);

#endif
