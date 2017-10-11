#include "hud.h"
#include <SFML/Graphics.hpp>
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"
#include "ball_entity.h"
#include "imgui.h"
#include "cursor.h"
#include <iostream>

const float PI_F = 3.1415927f;

EntityBall *ballHud = NULL;

Hud::Hud() {
    Subject::add_observer(this); // register for events
    dragging = false;
    canFling = canNudge = false;
    txtArrow.loadFromFile("data/arrow.png");
    txtBallNudge.loadFromFile("data/ball_nudge.png");
    moveCount = 0;
    fntCounter.loadFromFile("data/VCR_OSD_MONO.ttf");
    txtInstructionDrag.loadFromFile("data/instruction1.png");
    txtInstructionSpace.loadFromFile("data/instruction2.png");
    sprInstructionDrag.setTexture(txtInstructionDrag);
    sprInstructionSpace.setTexture(txtInstructionSpace);
    sprInstructionDrag.setOrigin(sf::Vector2f(txtInstructionDrag.getSize().x / 2.f, txtInstructionDrag.getSize().y / 2.f));
    sprInstructionSpace.setOrigin(sf::Vector2f(txtInstructionSpace.getSize().x / 2.f, txtInstructionSpace.getSize().y / 2.f));
    sprInstructionSpace.setScale(sf::Vector2f(0.7f, 0.7f));
    drawArrowOnBall = true;
}

void Hud::draw(sf::RenderWindow &window, sf::Vector2f camera, sf::Color levelColour) {
    sf::Vector2i mouseI = sf::Mouse::getPosition(window);
    sf::Vector2f mouse;
    mouse.x = (float)mouseI.x;
    mouse.y = (float)mouseI.y;

    // draw instructions
    if (SHOW_INITIAL_INSTRUCTIONS) {
        sprInstructionDrag.setPosition(sf::Vector2f(200.f - camera.x, 2200.f - camera.y));
        sprInstructionSpace.setPosition(sf::Vector2f(950.f - camera.x, 2260.f - camera.y));
        window.draw(sprInstructionDrag);
        //window.draw(sprInstructionSpace);
    }

    // draw mouse drag arrow
    bool onSand = false;
    if (ballHud) onSand = ballHud->is_on_sand();
    if (dragging && canFling) {
        // Calculate the speed the ball WILL be flung at if the player releases
        // the mouse button
        float speed = 0.f;
        sf::Vector2f dir = mouse - mouseDragStart;
        if (onSand) {
            speed = util::len(dir) / 25.f;
            speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED_NERF - 2.f);
        } else {
            speed = util::len(dir) / 15.f;
            speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED - 2.f);
        }

        // Calculate how much to scale the arrow
        // Arrow scale 1.0 == Ball speed BALL_MAX_LAUNCH_SPEED
        float scale = fmin(1.f, speed / BALL_MAX_LAUNCH_SPEED);

        // Draw arrow
        float ang = atan2f(dir.y, dir.x) * (180.f / PI_F);
        sf::Sprite sprArrow(txtArrow);
        sprArrow.setOrigin(sf::Vector2f(0.f, (float)txtArrow.getSize().y / 2.f));
        if (drawArrowOnBall)
            sprArrow.setPosition(ballRestPos - camera);
        else
            sprArrow.setPosition(mouseDragStart);
        sprArrow.setScale(sf::Vector2f(scale, 1.f));
        sprArrow.setRotation(ang);
        sprArrow.setColor(sf::Color(255, 255, 255, 128));
        window.draw(sprArrow);
    }

    // draw nudge indicator
    if (canNudge && !canFling) {
        sf::Vector2f mouse;
        mouse.x = (float)sf::Mouse::getPosition(window).x;
        mouse.y = (float)sf::Mouse::getPosition(window).y;
        sf::Vector2f dir = mouse - (ballRestPos - camera);
        float ang = atan2f(dir.y, dir.x) * (180.f / PI_F);
        sf::Sprite sprNudge(txtBallNudge);
        sprNudge.setOrigin(sf::Vector2f((float)txtBallNudge.getSize().x / 2.f, (float)txtBallNudge.getSize().y / 2.f));
        sprNudge.setPosition(ballRestPos - camera);
        sprNudge.setRotation(ang);
        sprNudge.setColor(sf::Color(levelColour.r, levelColour.g, levelColour.b, 128));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sprNudge.setColor(levelColour);
            sprNudge.setScale(sf::Vector2f(1.2f, 1.2f));
        }
        window.draw(sprNudge);
    }

    // set cursors
    if (canNudge) set_cursor(CURSOR_CROSS);
    if (canFling) set_cursor(CURSOR_RETICLE);
    if (!canNudge && !canFling) set_cursor(CURSOR_STOP);

    if (edit && ImGui::CollapsingHeader("Hud")) {
        ImGui::Checkbox("draw arrow on ball", &drawArrowOnBall);
    }
}

void Hud::on_notify(Event event, void *data) {
    if (event == EVENT_PLAYER_START_DRAG) {
        mouseDragStart = *((sf::Vector2f*)data);
        dragging = true;
    }
    if (event == EVENT_PLAYER_END_DRAG) {
        dragging = false;
        moveCount += 1;
    }
    if (event == EVENT_BALL_REST_POS || event == EVENT_BALL_MOVE) {
        ballRestPos = *((sf::Vector2f*)data);
    }
    if (event == EVENT_BALL_CHANGE_CAN_FLING) {
        canFling = *((bool*)data);
    }
    if (event == EVENT_BALL_CHANGE_CAN_NUDGE) {
        canNudge = *((bool*)data);
    }
    if (event == EVENT_PLAYER_END_DRAG) {
        sprInstructionDrag.setColor(sf::Color(255,255,255,0));
    }
}
