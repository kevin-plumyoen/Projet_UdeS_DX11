#pragma once
#include "HUD_En_Jeux.h"

#include <iomanip>

CHUD_En_Jeux::CHUD_En_Jeux(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer)
        : CMenu(std::move(fon), std::move(disp), std::move(gameRenderer))
        , text_options{ L"Chrono", L"", L"Vitesse" }
    {
        selectedOption = 0;
        billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ .8f,-1.f }, Vec2{ .3f, .2f }, pDispositif));
        billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ -.7f,-1.f }, Vec2{ .3f, .2f }, pDispositif));
        billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.1f,.8f }, Vec2{ .3f, .2f }, pDispositif));
        afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
        afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
        afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
        std::reverse(text_options.begin(), text_options.end());
    }
    void CHUD_En_Jeux::afficher() {
        for (int i = 0; i < afficheurs.size(); ++i)
        {
            renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(i).get(), afficheurs.at(i)->GetTextureView()));
            afficheurs.at(i)->Ecrire(text_options.at(i), *pFont);
        }
    }

    void CHUD_En_Jeux::update_chrono(int min, int sec)
    {
        std::wstringstream stream;
        stream << std::setw(2) << std::setfill(L'0') << min
               << ":"
               << std::setw(2) << std::setfill(L'0') << sec;

        text_options.at(2) = stream.str();
    }

    void CHUD_En_Jeux::update_vitesse(float vitesse)
    {
        const float floored_speed = std::floor(vitesse);

        std::wstringstream stream;
        stream << static_cast<int>(floored_speed) << " mo/s";

        text_options.at(0) = stream.str();
    }

    void CHUD_En_Jeux::gerer_input(const PM3D::CDIManipulateur& inp)
    {
        if (inp.ToucheAppuyee(KeyboardKey::escape))
            EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuAfficher));
    }