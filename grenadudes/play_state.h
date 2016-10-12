#pragma once

// a round of gameplay between a player and an ai

#include "state.h"
#include "hud.h"

class StatePlay : public State {
public:
    StatePlay(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();
private:
    Hud hud; // hud renderer
};