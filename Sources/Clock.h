#ifndef DEF_CLOCK_H
#define DEF_CLOCK_H

#include <chrono>

using GameClock = std::chrono::high_resolution_clock;
using FrameDuration = typename GameClock::duration;

#endif