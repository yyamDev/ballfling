#pragma once

// plays audio
#include "observer.h"
#include <SFML/Audio.hpp>

enum track {
    MUSIC_TRACK_MENU,
    MUSIC_TRACK_A,
    MUSIC_TRACK_B,
    MUSIC_TRACK_C,
    MUSIC_TRACK_COUNT,
};

class Audio : public Observer {
public:
    Audio();
    void stop_all_music();
    void update_all_music_volume();

    void tick_game_music();
    sf::Music *track_to_music(track track_enum);
    void go_to_next_game_track();

    void on_notify(Event event, void *data);

    void reload_sfx();
private:
    sf::Clock clkHit;
    sf::Clock clkHitSandy;

    track currentlyPlaying;
    track wasCurrentlyPlaying;
    sf::Music musMenu;
    sf::Music musGameA;
    sf::Music musGameB;
    sf::Music musGameC;
};
