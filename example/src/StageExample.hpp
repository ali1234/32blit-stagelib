#pragma once

#include "Stage.hpp"


class StageExample : public Timed {
    public:
        StageExample() : Timed(4000) {}
        void update(uint32_t time, ButtonState buttons) override;
        void render() override;
};
