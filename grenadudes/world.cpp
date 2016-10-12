#include "world.h"
#include <assert.h>
#include <iostream>
#include "subject.h"

World::World(sf::RenderWindow &window) {
    this->window = &window;
}

World::~World() {
    if (entities.size() > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            Entity *ptr = (*it);
            it = entities.erase(it);
            delete ptr;
        }
    }
    assert(entities.size() == 0);
}

void World::event(sf::Event &e) {
    for (auto &entity : entities) {
        entity->event(e);
    }
}

void World::tick() {
    size_t vectorLength = entities.size(); // size might change as we go through
    if (vectorLength > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            (*it)->tick(entities);
            if ((*it)->remove) {
                Entity *ptr = (*it);
                it = entities.erase(it);
                delete ptr;
            } else ++it;
        }
    }

    while (entityAddQueue.size() > 0) {
        entities.push_back(entityAddQueue.front());
        entityAddQueue.pop();
    }
}

void World::draw() {
    for (auto &entity : entities) {
        entity->draw(*window);
    }
}

sf::Vector2i World::get_mouse_position() {
    return sf::Mouse::getPosition(*window);
}

void World::add_entity(Entity *entity) {
    entity->set_world_ptr(this);
    entityAddQueue.push(entity);
}

int World::remove_entity(std::string tag) {
    int count = 0;
    size_t vectorLength = entities.size(); // size might change as we go through
    if (vectorLength > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            if ((*it)->get_tag() == tag) {
                Entity *ptr = (*it);
                it = entities.erase(it);
                delete ptr;
                count ++;
            }
            else ++it;
        }
    }
    return count;
}

void World::on_notify(Event event, void *data) {
}