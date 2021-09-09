#pragma once

#include "32blit.hpp"

#include <cstdint>

class ButtonState {
    public:
        ButtonState(uint32_t buttons=0, uint32_t delta=0) : buttons(buttons), delta(delta) {}
        uint32_t up(uint32_t mask) const {return mask & ~buttons;}
        uint32_t down(uint32_t mask) const {return mask & buttons;}
        uint32_t released(uint32_t mask) const {return up(mask) & delta;}
        uint32_t pressed(uint32_t mask) const {return down(mask) & delta;}
        void update(uint32_t new_buttons) {delta = buttons ^ new_buttons; buttons = new_buttons;}
        static constexpr uint32_t ABXY = blit::Button::A | blit::Button::B | blit::Button::X | blit::Button::Y;
    private:
        uint32_t buttons, delta;
};

