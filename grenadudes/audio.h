#pragma once

// plays audio
#include "observer.h"
#include <SFML/Audio.hpp>

class Audio : public Observer {
public:
    Audio();
    void on_notify(Event event, void *data);
private:
    sf::SoundBuffer bufSplash;
    sf::Sound sndSplash;
};