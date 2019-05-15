#pragma once

// game pause state

#include "state.h"
#include "observer.h"
#include "subject.h"
#include "SFGUI/Scale.hpp"
#include <SFML/Graphics.hpp>

class World;

class StatePause : public State, public Observer, public Subject {
public:
    StatePause(World *world, sf::Color clear);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);
    sf::Color get_clear_colour();
private:
    sf::Color clear;
};
