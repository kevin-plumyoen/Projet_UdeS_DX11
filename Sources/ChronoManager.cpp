#include "ChronoManager.h"

#include "HUD_Manager.h"

ChronoManager::ChronoManager(CHUD_Manager& hud)
: hud{hud}
, acc{}{


}

void ChronoManager::update(FrameDuration dt) {
    acc += dt;

    //const std::chrono::minutes minutes = std::chrono::duration_cast<std::chrono::minutes>(acc);
    const std::chrono::seconds sec_duration = std::chrono::duration_cast<std::chrono::seconds>(acc);
    const int minutes = static_cast<int>(sec_duration.count()) / 60;
    const int secondes = static_cast<int>(sec_duration.count()) - (minutes * 60);

    hud.update_game_hud_chrono(minutes, secondes);
}