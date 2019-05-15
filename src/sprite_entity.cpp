#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "sprite_entity.h"
#include "ball_entity.h"
#include "fireworks_entity.h"
#include "world.h"

EntitySprite::EntitySprite(std::string filename, float x, float y, float scale, float angle, bool fireworks) :
    doFireworks(fireworks)
{
    txt.loadFromFile(filename);
    spr.setTexture(txt);
    spr.setOrigin(txt.getSize().x / 2.f, txt.getSize().y / 2.f);
    position.x = x;
    position.y = y;
    spr.setPosition(x, y);
    spr.setScale(scale, scale);
    spr.setRotation(angle);
    tag = filename;
    collisionRadius = 32.f;
}

EntitySprite::EntitySprite(std::string filename, float x, float y, float scale, float angle) :
    EntitySprite(filename, x, y, scale, angle, false)
{
}

EntitySprite::EntitySprite(std::string filename, float x, float y) :
    EntitySprite(filename, x, y, 1.f, 0.f)
{
}

EntitySprite::EntitySprite(std::string filename, float x, float y, float scale) :
    EntitySprite(filename, x, y, scale, 0.f)
{
}

void EntitySprite::event(sf::Event &e)
{
}

void EntitySprite::tick(std::vector<Entity*> &entities)
{
    spr.setPosition(position.x, position.y);

    if (!doFireworks || spawnedFireworks)
        return;

    // Get pointer to player entity
    if (!ball) {
        for (Entity *e : entities) {
            if (e->get_tag() == "ball") {
                ball = (EntityBall*)e;
                break;
            }
        }
    }

    if (ball && intersects(*ball)) {
        world->add_entity(new EntityFireworks(position, false));
        spawnedFireworks = true;
    }
}

void EntitySprite::draw(sf::RenderWindow &window)
{
    spr.move(-world->camera);
    window.draw(spr);
    spr.move(world->camera);
}
