#include "state.h"
#include "build_options.h"
#include "debug_draw.h"
#include "imgui.h"

std::stack<State*> State::states;

State::State(World *world, std::string name) : world(world), name(name)
{
}

sf::Color State::get_clear_colour()
{
    return DEFAULT_CLEAR_COLOUR;
}

std::string State::get_name()
{
    return name;
}

void State::reset() {
    // Reset the entire state stack freeing all memory used.
    while (!State::states.empty()) {
        State::states.top()->on_lose_focus();
        delete State::states.top();
        State::states.pop();
    }
}

void State::change_state(State *state) {
    if (!State::states.empty()) {
        State::states.top()->on_lose_focus();
        delete State::states.top();
        State::states.pop();
    }
    State::states.push(state);
    state->on_gain_focus();
}

void State::push_state(State *state) {
    if (!State::states.empty()) {
        State::states.top()->on_lose_focus();
    }
    State::states.push(state);
    State::states.top()->on_gain_focus();
}

void State::pop_state() {
    if (!State::states.empty()) {
        State::states.top()->on_lose_focus();
        delete State::states.top();
        State::states.pop();
            if (!State::states.empty())
                State::states.top()->on_gain_focus();
    }
}

void State::event_current(sf::Event &event) {
    if (!State::states.empty())
        State::states.top()->on_event(event);
}

void State::tick_current() {
    if (!State::states.empty())
        State::states.top()->on_tick();
}

void State::draw_current(sf::RenderWindow &window) {
    if (!State::states.empty())
        State::states.top()->on_draw(window);

    if (edit && ImGui::CollapsingHeader("state")) {
        ImGui::Text("Size of state stack: %i", State::states.size());
    }
}

void State::draw_ui_current(sf::RenderWindow &window) {
    if (!State::states.empty())
        State::states.top()->on_draw_ui(window);
}

State* State::get_current() {
    if (!State::states.empty())
        return State::states.top();
    return NULL;
}
