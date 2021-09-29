// StageLib - Stage.hpp
// Copyright (c) 2021 Alistair Buxton <a.j.buxton@gmail.com>

#pragma once

#include <memory>
#include "ButtonState.hpp"

class Stage;

typedef std::shared_ptr<Stage> StagePtr;

class Stage : public std::enable_shared_from_this<Stage> {
    public:
        Stage() { _count++; _all_time_count++;}
        virtual ~Stage() { _count--; }

        static int count() { return _count; }
        static int all_time_count() { return _all_time_count; }

        virtual void update(uint32_t, ButtonState) {}
        virtual void render() {}

        StagePtr next();
        bool finished() const;
        bool changed() const;

    protected:
        void finish(StagePtr next);
        void change(StagePtr next);

    private:
        bool _finished = false;
        bool _changed = false;
        StagePtr _next = nullptr;
        inline static int _count = 0;
        inline static int _all_time_count = 0;
};


class Timed : public Stage {
    public:
        Timed(uint32_t duration) : Stage(), duration(duration) {}

        void update(uint32_t time, ButtonState buttons) override;

    protected:
        bool expired() const;
        float progress() const;

        uint32_t duration, elapsed = 0;
};


class Fade : public Timed {
    public:
        Fade(StagePtr a, StagePtr b, uint32_t duration=250, blit::Pen color=blit::Pen(0, 0, 0))
                : Timed(duration/2), a(a), b(b), color(color) {}
        Fade(StagePtr a, StagePtr b, blit::Pen color, uint32_t duration=250)
                : Fade(a, b, duration, color) {}

        void update(uint32_t time, ButtonState buttons) override;
        void render() override;

    protected:
        StagePtr a, b;
        blit::Pen color;
        bool direction = false;
};


class Modal : public Stage {
    public:
        Modal(StagePtr background, bool update_background)
                : Stage(), previous(background), background(background), update_background(update_background) {}
        Modal(StagePtr previous, StagePtr background, bool update_background)
                : Stage(), previous(previous), background(background), update_background(update_background) {}

        void update(uint32_t time, ButtonState buttons) override;
        void render() override;

    protected:
        StagePtr previous;
        StagePtr background;
        bool update_background;
};