// StageLib - SoundMgr.cpp
// Copyright (c) 2022 Alistair Buxton <a.j.buxton@gmail.com>

#include <cstring>

#include "32blit.hpp"

#include "SoundMgr.hpp"

void audio_bouncer(blit::AudioChannel &channel) {
    if (channel.user_data) {
        ((SoundMgr *) (channel.user_data))->wave_callback(channel);
    } else {
        channel.wave_buffer_callback = nullptr;
        memset(channel.wave_buffer, 0, 128);
    }
}

SoundMgr::SoundMgr(Channel c) : c(c), sample_ptr(nullptr), sample_end(nullptr) {
    blit::channels[c].user_data = this;
    blit::channels[c].waveforms = blit::Waveform::WAVE;
    blit::channels[c].wave_buffer_callback = &audio_bouncer;
}

SoundMgr::~SoundMgr() {
    blit::channels[c].user_data = nullptr;
}

void SoundMgr::wave_callback(blit::AudioChannel &channel)
{
    unsigned remaining = sample_end - sample_ptr;
    if (remaining > 64) remaining = 64;
    for (unsigned n = 0; n < remaining; n++)
        channel.wave_buffer[n] = sample_ptr[n]<<vol;
    for (unsigned n = remaining; n < 64; n++)
        channel.wave_buffer[n] = 0;
    sample_ptr += remaining;
    if (remaining == 0) {
        channel.off();
    }
}

void SoundMgr::play(const int8_t *data, uint32_t length) {
    vol = 5;
    sample_ptr = data;
    sample_end = data + length;
    blit::channels[c].trigger_attack();
}


SoundMgr& SoundMgr::get(const Channel c) {
    static SoundMgr *sound_mgrs[3];

    if (!sound_mgrs[c])
        sound_mgrs[c] = new SoundMgr(c);
    return *sound_mgrs[c];
}
