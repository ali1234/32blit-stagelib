// StageLib example - Main.cpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#include <cinttypes>
#include <cstring>

#ifndef NDEBUG
#ifdef TARGET_32BLIT_HW // For mallinfo
#include <malloc.h>
#endif
#endif

#include "32blit.hpp"

#include "Stage.hpp"
#include "StageExample.hpp"

#ifndef NDEBUG
#include "engine/version.hpp"
#include "FancyText.hpp"

uint32_t utime = 0, rtime = 0;
blit::GameMetadata metadata;
#endif

static ButtonState buttons;
static uint32_t prev_update;

static StagePtr stage = nullptr;


void init() {
#ifndef NDEBUG
    metadata = blit::get_metadata();
#endif
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
#ifndef NDEBUG
        utime = blit::now() - time;
#endif
    }
}

void render(uint32_t /* time */) {
#ifndef NDEBUG
    char message[80];
    uint32_t before = blit::now();
#endif

    stage->render();

#ifndef NDEBUG
    rtime = blit::now() - before;
#ifdef TARGET_32BLIT_HW
    struct mallinfo mi = mallinfo();
    sprintf(message, "%s U:%3" PRIu32 "ms R:%3" PRIu32 "ms H:%5d U:%5d F:%5d S:%d (%d)",
            metadata.version, utime, rtime, mi.arena>>0, mi.uordblks>>0, mi.fordblks>>0,
            Stage::count(), Stage::all_time_count());
#else
    sprintf(message, "%s %s - U:%3" PRIu32 "ms R:%3" PRIu32 "ms S:%d (%d)",
            metadata.title, metadata.version, utime, rtime,
            Stage::count(), Stage::all_time_count());
#endif
    fancy_text_fixed(message, blit::Point(7, 228%16));
#endif
}
