#include <stdio.h>
#include "home_theater_facade.h"

int main() {
    HomeTheaterFacade *home_theater = home_theater_facade_create();
    
    printf("===== Facade Pattern Demo =====\n\n");
    
    printf("1. Watch movie using facade:\n");
    home_theater_facade_watch_movie(home_theater, "Inception");
    
    printf("2. End movie using facade:\n");
    home_theater_facade_end_movie(home_theater);
    
    printf("3. Listen to music using facade:\n");
    home_theater_facade_listen_to_music(home_theater, "Dark Side of the Moon");
    
    printf("4. Stop music using facade:\n");
    home_theater_facade_stop_music(home_theater);
    
    printf("\n===== Without facade =====\n");
    printf("You would need to call multiple subsystem functions:\n");
    printf("// This code CANNOT compile - subsystem functions are hidden!\n");
    printf("// projector_on(home_theater->projector);\n");
    printf("// amplifier_set_volume(home_theater->amplifier, 5);\n");
    
    home_theater_facade_destroy(home_theater);
    return 0;
}
