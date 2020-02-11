#ifndef DEF_GAME_CONTROLLER_H
#define DEF_GAME_CONTROLLER_H

#include "Gestionnaires.h"

class GameController {
    const GestionnaireTag& tags;
public:
    GameController(const GestionnaireTag& tags);
    void check_win_condition();
};

#endif