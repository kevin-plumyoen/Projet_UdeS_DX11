#ifndef DEF_CHRONO_MANAGER_H
#define DEF_CHRONO_MANAGER_H

#include "Clock.h"

class CHUD_Manager;

class ChronoManager {
    CHUD_Manager& hud;
    FrameDuration acc;
public:
    ChronoManager(CHUD_Manager& hud);
    void update(FrameDuration dt);
};

#endif