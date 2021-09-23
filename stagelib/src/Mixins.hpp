#pragma once

#include "32blit.hpp"

class Positioned {
    public:
        Positioned(int x, int y, int w, int h) : position(x, y, w, h) {}

        void top(int y) {position.y = y;}
        void bottom(int y) {position.y = 240 - y - position.h;}
        void left(int x) {position.x = x;}
        void right(int x) {position.x = 320 - x - position.w;}

    protected:
        blit::Rect position;
};


class Framed : public Positioned {
    public:
        Framed(int x, int y, int w, int h, int offset = 0,
               blit::Pen colour=blit::Pen(255, 255, 255),
               blit::Pen shadow=blit::Pen(0, 0, 0, 50)
        ) : Positioned(x, y, w, h), offset(offset), colour(colour), shadow(shadow) {}

    protected:
        void draw_frame_h(int x, int y, int length);
        void draw_frame_v(int x, int y, int length);
        void draw_frame(bool draw_top = true);

    private:
        int offset;
        blit::Pen colour, shadow;
};