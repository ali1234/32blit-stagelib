#pragma once

#include <string_view>

#include "32blit.hpp"

void fancy_text_ex(std::string_view message, const blit::Point &r, bool dim, bool fixed, blit::TextAlign align);

inline void fancy_text(std::string_view message, const blit::Point &r, bool dim = false, blit::TextAlign align = blit::TextAlign::left) {
    fancy_text_ex(message, r, dim, true, align);
}

inline void fancy_text_fixed(std::string_view message, const blit::Point &r, bool dim = false, blit::TextAlign align = blit::TextAlign::left) {
    fancy_text_ex(message, r, dim, false, align);
}
