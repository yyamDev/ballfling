#include <iostream>
#include "cursor.h"
#include "util.h"

namespace _cursors {
    sf::Texture txtCursorReticle;
    sf::Texture txtCursorCross;
    sf::Texture txtCursorPointer;
    sf::Texture txtCursorStop;
    sf::Sprite sprCursor; // Current cursor
    bool draw;
}

void load_cursor_textures()
{
    _cursors::txtCursorReticle.loadFromFile("data/cursor_drag.png");
    _cursors::txtCursorCross.loadFromFile("data/cursor_nudge.png");
    _cursors::txtCursorPointer.loadFromFile("data/cursor_pointer.png");
    _cursors::txtCursorStop.loadFromFile("data/cursor_stop.png");
    _cursors::sprCursor.setOrigin(13.f, 13.f); // 26 X 26 px cursors
}

void draw_cursor(sf::RenderWindow &window)
{
    if (!_cursors::draw) return;

    sf::Vector2i mouseI = sf::Mouse::getPosition(window);
    _cursors::sprCursor.setPosition((float)mouseI.x, (float)mouseI.y);

    window.draw(_cursors::sprCursor);
}

void set_cursor(sf::Texture *cursorTexture)
{
    _cursors::sprCursor.setTexture(*cursorTexture);
}

void set_cursor_visible(bool visible)
{
    _cursors::draw = visible;
}
