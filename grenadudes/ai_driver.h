#pragma once

// ai dude driver

#include "dude_driver.h"
#include "dude_entity.h"
#include "subject.h"
#include <vector>
#include "terrain_grid.h"
#include "nav_graph.h"

class EntityTerrain;

enum AiState {
    AI_STATE_IDLE,
    AI_STATE_FLEE
};

class AiDriver : public DudeDriver, public Subject, public Observer {
public:
    AiDriver(EntityDude *dude, EntityTerrain *terrain);
    ~AiDriver();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void AiDriver::on_notify(Event event, void *data);
private:
    EntityTerrain *terrain;

    // terrain grid approximation
    TerrainGrid terrainGrid;
    sf::Clock clockRecalcTerrainGrid;
    int quad; // which quadrant to update
    bool showGrid;

    // nav graph
    NavGraph navGraph;
    bool showNav;
    std::vector<sf::Vector3f> currentPath;
    sf::Vector2f prevPosition;
    sf::Clock stuckTimer;

    // decisions
    AiState state;
    Entity *grenade;
    Entity* get_cloeset_grenade(std::vector<Entity*> &entities);
};