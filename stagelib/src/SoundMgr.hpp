#pragma once

class SoundMgr {
    public:
        enum Channel {
            Left = 0,
            Right = 1,
            Misc = 2,
        };
        static SoundMgr& get(Channel c);
        static SoundMgr& L() { return get(Channel::Left); }
        static SoundMgr& R() { return get(Channel::Right); }
        template<uint32_t N> void play(const int8_t (& array)[N]) { play(array, N); }
        void play(const int8_t *data, uint32_t length);
        void wave_callback(blit::AudioChannel &channel);
    protected:
        uint8_t vol = 0;
        SoundMgr(Channel c);
        ~SoundMgr();
        const Channel c;
        const int8_t *sample_ptr;
        const int8_t *sample_end;
};
