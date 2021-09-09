// StageLib example - StageExample.cpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#include "StageExample.hpp"
#include "FancyText.hpp"


void StageExample::update(uint32_t time, ButtonState buttons) {
    Timed::update(time, buttons);
    if (!finished()) {
        if (buttons.pressed(ButtonState::ABXY)) {
            finish(std::make_shared<Fade>(shared_from_this(), shared_from_this()));
        } else if (expired()) {
            finish(std::make_shared<Fade>(shared_from_this(), shared_from_this()));
        }
    }
}

void StageExample::render() {
    blit::screen.pen = blit::Pen(200, 20, 20);
    blit::screen.clear();
    fancy_text("Block Game\nPress Button", blit::Point(160, 120), false, blit::TextAlign::center_center);
}