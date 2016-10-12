#include "nav_graph.h"
#include "terrain_grid.h"
#include "debug_draw.h"
#include "util.h"
#include <iostream>
#include <set>

NavNode::NavNode() {
    NavNode(sf::Vector2f(0.f, 0.f));
}

NavNode::NavNode(sf::Vector2f worldPosition) {
    this->worldPosition = worldPosition;
    flag = false;
    g = h = 0;
    parent = NULL;
    visited = false;
    jump = false;
}

std::vector<NavNode> generate_nav_graph(TerrainGrid &grid) {
    std::vector<NavNode> graph;
    graph.reserve(1000); // so pointers to nodes work (stop reallocation)
    for (unsigned int y = 0; y<grid.get_grid_height(); ++y) {
        for (unsigned int x = 0; x<grid.get_grid_width(); ++x) {
            sf::Vector2u gridPos(x, y);
            sf::Vector2f worldPos((float)(x * grid.get_cell_size()), (float)(y * grid.get_cell_size()));
            float cellSize = (float)grid.get_cell_size();
            if (grid.is_solid(grid.get_index(gridPos))) {
                sf::Vector2u aboveOne(gridPos.x, gridPos.y - 1);
                sf::Vector2u aboveTwo(gridPos.x, gridPos.y - 2);
                if (!grid.is_solid(grid.get_index(aboveOne)) && !grid.is_solid(grid.get_index(aboveTwo))) {
                    NavNode node = NavNode(sf::Vector2f(worldPos.x + cellSize / 2.f, worldPos.y - cellSize / 2.f));
                    graph.push_back(node);
                    // add edges
                    for (auto &other : graph) {
                        if (std::fabs(node.worldPosition.x - other.worldPosition.x) == cellSize) {
                            if (std::fabs(node.worldPosition.y - other.worldPosition.y) == cellSize || node.worldPosition.y - other.worldPosition.y == 0.f) {
                                // add walking edge
                                graph[graph.size() - 1].walkingEdge.push_back(&other);
                                other.walkingEdge.push_back(&graph[graph.size() - 1]);
                            }
                            if (node.worldPosition.y - other.worldPosition.y > cellSize) {
                                // add falling edge to other
                                other.fallingEdge.push_back(&graph[graph.size() - 1]);
                                if (node.worldPosition.y - other.worldPosition.y < cellSize * 4.f) {
                                    // add jumping edge to us
                                    graph[graph.size() - 1].jumpingEdge.push_back(&other);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return graph;
}

void draw_nav_graph(sf::RenderWindow &window, std::vector<NavNode> &navGraph) {
    for (auto &node : navGraph) {
        float rad = 4.f;
        sf::CircleShape circle(rad);
        circle.setPosition(node.worldPosition);
        circle.setOrigin(sf::Vector2f(rad, rad));
        if (node.flag)
            circle.setFillColor(sf::Color(255, 0, 255, 128));
        else
            circle.setFillColor(sf::Color(0, 0, 255, 128));
        window.draw(circle);
        float size = 12.f;
        for (auto &walk : node.walkingEdge) {
            draw_vector(node.worldPosition, walk->worldPosition - node.worldPosition, size, sf::Color::Blue, window);
        }
        for (auto &jump : node.jumpingEdge) {
            draw_vector(node.worldPosition, jump->worldPosition - node.worldPosition, size, sf::Color::Yellow, window);
        }
        for (auto &fall : node.fallingEdge) {
            draw_vector(node.worldPosition, fall->worldPosition - node.worldPosition, size, sf::Color::Red, window);
        }
    }
}

NavNode *get_closest_node(NavGraph &navGraph, sf::Vector2f position) {
    float closestDist = 10000000.f;
    NavNode *closestNode = NULL;
    for (NavNode &node : navGraph) {
        float dist = (node.worldPosition.x - position.x) * (node.worldPosition.x - position.x) + (node.worldPosition.y - position.y) * (node.worldPosition.y - position.y);
        if (dist < closestDist) {
            closestDist = dist;
            closestNode = &node;
        }
    }
    return closestNode;
}

bool find_path(NavNode *start, NavNode *goal, NavGraph &navGraph, std::vector<sf::Vector3f> *path) {
    for (auto &node : navGraph) {
        node.visited = false;
        node.flag = false;
    }
    return _find_path(start, start, goal, path);
}

bool _find_path(NavNode *start, NavNode *current, NavNode *goal, std::vector<sf::Vector3f> *path) { // A*
    static std::set<NavNode*> open;
    static std::set<NavNode*> visited;
    const int G_WALK = 10;
    const int G_JUMP = 15;
    const int G_FALL = 20;

    // cleanup
    if (start == current) { // only run at the start
        open.clear();
        visited.clear();
    }

    if (current == goal) {
        NavNode *currentBack = goal;
        path->clear();
        while (currentBack != start) {
            // add path in reverse for easier iteration later
            if (currentBack->jump)
                path->push_back(sf::Vector3f(currentBack->worldPosition.x, currentBack->worldPosition.y, 1.f));
            else
                path->push_back(sf::Vector3f(currentBack->worldPosition.x, currentBack->worldPosition.y, 0.f));
            currentBack->flag = true;
            currentBack = currentBack->parent;
        }
        return true;
    }

    // add all walkable nodes to open list
    for (auto &walk : current->walkingEdge) {
        if (visited.find(walk) == visited.end()) { // if node hasn't been visited
            walk->g = G_WALK;
            walk->h = 0; // todo -> add better heuristic
            walk->parent = current;
            if (open.find(walk) == open.end())
                open.insert(walk);
        }
    }

    // add all jumpable nodes to open list
    for (auto &jump : current->jumpingEdge) {
        if (visited.find(jump) == visited.end()) { // if node hasn't been visited
            jump->g = G_JUMP;
            jump->h = 0; // todo -> add better heuristic
            jump->parent = current;
            jump->jump = true;
            if (open.find(jump) == open.end())
                open.insert(jump);
        }
    }

    // add all fallable nodes to open list
    for (auto &fall : current->fallingEdge) {
        if (visited.find(fall) == visited.end()) { // if node hasn't been visited
            fall->g = G_FALL;
            fall->h = 0; // todo -> add better heuristic
            fall->parent = current;
            if (open.find(fall) == open.end())
                open.insert(fall);
        }
    }

    // remove this node from open list & add to visited list
    if (open.find(current) != open.end())
        open.erase(open.find(current));
    visited.insert(current);

    // chose next node with lowest f cost
    int lowestF = 1000;
    NavNode *next = NULL;
    for (auto &node : open) {
        if (node->g + node->h < lowestF) {
            lowestF = node->g + node->h;
            next = node;
        }
    }
    if (next == NULL)
        return false; // dead end or no path exists
    else
        return _find_path(start, next, goal, path);
}