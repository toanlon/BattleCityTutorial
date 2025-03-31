#include "ImpTimer.h"

ImpTimer::ImpTimer() {
    start_tick_ = 0;
    paused_tick_ = 0;
    is_paused_ = false;
    is_started_ = false;
}

ImpTimer::~ImpTimer() {}

void ImpTimer::start() {
    is_started_ = true;
    is_paused_ = false;
    start_tick_ = SDL_GetTicks();
}

int ImpTimer::get_ticks() {
    if(is_started_ == true) {
        if(is_paused_ == true) {
            return paused_tick_;
        }
        else return SDL_GetTicks() - start_tick_;
    }

    return 0;
}

