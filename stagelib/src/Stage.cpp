// StageLib - Stage.cpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#include "Stage.hpp"


void Stage::change(StagePtr next) {
    _next = next;
    _changed = true;
}

void Stage::finish(StagePtr next) {
    change(next);
    _finished = true;
}

bool Stage::changed() const {
    return _changed;
}

bool Stage::finished() const {
    return _finished;
}

StagePtr Stage::next() {
    _changed = false;
    StagePtr __next = _next;
    _next = nullptr;
    return __next;
}


void Timed::update(uint32_t time, ButtonState /* buttons */) {
    elapsed += time;
}

bool Timed::expired() const {
    return elapsed > duration;
}

float Timed::progress() const {
    float tmp = (float) elapsed / duration;
    if (tmp < 0) return 0;
    if (tmp > 1) return 1;
    return tmp;
}


void Fade::update(uint32_t time, ButtonState buttons) {
    Timed::update(time, buttons);
    if (!finished()) {
        if (expired()) {
            if (direction) {
                finish(b);
            } else {
                direction = true;
                elapsed = 0;
            }
        }
    }
}

void Fade::render() {
    if (direction) {
        b->render();
        color.a = 255*sinf((1.0f - progress())*3.141f*0.5f);
        blit::screen.pen = color;
        blit::screen.clear();
    } else {
        a->render();
        color.a = 255*sinf(progress()*3.141f*0.5f);
        blit::screen.pen = color;
        blit::screen.clear();
    }
}


void Modal::update(uint32_t time, ButtonState /* buttons */) {
    if (!finished()) {
        if (update_background) {
            background->update(time, 0);
            if (background->changed()) background = background->next();
        }
    }
}

void Modal::render() {
    background->render();
    blit::screen.pen = blit::Pen(0.0f, 0.0f, 0.0f, 0.5f);
    blit::screen.clear();
}
