// StageLib - Mixins.cpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#include "Mixins.hpp"


void Framed::draw_frame_h(int x, int y, int length) {
    blit::screen.pen = blit::Pen(colour);
    blit::screen.rectangle(blit::Rect(x+1, y, length, 2));
    blit::screen.pen = blit::Pen(shadow);
    blit::screen.rectangle(blit::Rect(x+2, y+1, length, 2));
}

void Framed::draw_frame_v(int x, int y, int length) {
    blit::screen.pen = blit::Pen(colour);
    blit::screen.rectangle(blit::Rect(x, y+1, 2, length));
    blit::screen.pen = blit::Pen(shadow);
    blit::screen.rectangle(blit::Rect(x+1, y+2, 2, length));
}

void Framed::draw_frame(bool draw_top) {
    int x = position.x - 1 - offset;
    int y = position.y - 1 - offset;
    int w = position.w + (2*offset);
    int h = position.h + (2*offset);

    if (draw_top) draw_frame_h(x, y, w);
    draw_frame_v(x, y+(draw_top?0:2), h-(draw_top?0:2));
    draw_frame_h(x, y+h, w);
    draw_frame_v(x+w, y+(draw_top?0:2), h-(draw_top?0:2));
}