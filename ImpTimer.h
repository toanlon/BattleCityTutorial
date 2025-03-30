#ifndef IMP_TIMER_H
#define IMP_TIMER_H_

#include "CommonFuntion.h"

class ImpTimer
{
    private :
        int start_tick_;
        int paused_tick_;

        bool is_paused_;
        bool is_started_;

    public:
        ImpTimer();
        ~ImpTimer();

        void start();
        void stop();
        void paused();
        void unpaused();

        int get_ticks();

        bool is_started();
        bool is_paused();

};



#endif  IMP_TIMER_H
