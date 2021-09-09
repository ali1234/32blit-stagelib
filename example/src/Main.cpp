// StageLib example - Main.cpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>. All rights reserved.

#include <cinttypes>
#include <cstring>

#ifdef TARGET_32BLIT_HW // For mallinfo
#include <malloc.h>
#endif

#include "32blit.hpp"
#include "engine/version.hpp"

#include "Stage.hpp"
#include "StageExample.hpp"
#include "FancyText.hpp"


static ButtonState buttons;
static uint32_t prev_update;

static StagePtr stage = nullptr;

uint32_t utime = 0, rtime = 0;

blit::GameMetadata metadata;

void init() {
    metadata = blit::get_metadata();
    blit::set_screen_mode(blit::ScreenMode::hires);
    stage = std::make_shared<Fade>(std::make_shared<Stage>(), std::make_shared<StageExample>());
    buttons.update(blit::buttons);
    prev_update = blit::now();

    // work around for https://github.com/32blit/32blit-sdk/issues/607
    blit::channels[7].waveforms = blit::Waveform::WAVE;
    blit::channels[7].wave_buffer_callback = nullptr;
    memset(blit::channels[7].wave_buffer, 0, 128);
    blit::channels[7].trigger_attack();
}

void update(uint32_t /*time*/) {
    uint32_t time = blit::now(); // firmware delivers nonsense in the time argument.
    uint32_t time_delta = time - prev_update;
    if (time_delta > 100) time_delta = 20;
    if (time_delta > 0) {
        buttons.update(blit::buttons);
        stage->update(time_delta, buttons);
        if (stage->changed()) stage = stage->next();
        prev_update = time;
        utime = blit::now() - time;
    }
}

void render(uint32_t /* time */) {
    char message[80];
    uint32_t before = blit::now();
    stage->render();
    uint32_t after = blit::now();
    rtime = after - before;

#ifdef TARGET_32BLIT_HW
    struct mallinfo mi = mallinfo();
    sprintf(message, "%s U:%3" PRIu32 "ms R:%3" PRIu32 "ms H:%5d U:%5d F:%5d S:%d", metadata.version, utime, rtime, mi.arena>>0, mi.uordblks>>0, mi.fordblks>>0, Stage::count());
#else
    sprintf(message, "%s %s - U:%3" PRIu32 "ms R:%3" PRIu32 "ms S:%d", metadata.title, metadata.version, utime, rtime, Stage::count());
#endif
    fancy_text_fixed(message, blit::Point(7, 228%16));
}
