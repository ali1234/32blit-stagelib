// StageLib - FancyText.cpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#include "FancyText.hpp"


void fancy_text_ex(std::string_view message, const blit::Point &p, bool dim, bool fixed, blit::TextAlign align) {
    blit::screen.pen = blit::Pen(0, 0, 0, 127);
    blit::screen.text(message, blit::minimal_font, p + blit::Point(1, 1), fixed, align);
    if (dim) {
        blit::screen.pen = blit::Pen(127, 127, 127, 255);
    } else {
        blit::screen.pen = blit::Pen(255, 255, 255, 255);
    }
    blit::screen.text(message, blit::minimal_font, p, fixed, align);
}