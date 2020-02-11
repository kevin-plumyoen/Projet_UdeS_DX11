#include "Endgame_Menu.h"

#include <iomanip>

CEndgame_Menu::CEndgame_Menu(Font *fon, PM3D::CDispositifD3D11 *disp, Renderer *gameRenderer, ID3D11ShaderResourceView* fond)
    : CMenu(std::move(fon), std::move(disp), std::move(gameRenderer))
    , text_options{ L"Bravo :" ,L"Chrono" }
    , fond_men{ fond }
{
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,-0.2f }, Vec2{ .5f, .4f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.0f,-0.4f }, Vec2{ .5f, .4f }, pDispositif));
    billboard_texte.emplace_back(std::make_unique<CScreenBillboard>(Vec2{ 0.f,0.f }, Vec2{ 1.f, 1.f }, pDispositif));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
    afficheurs.emplace_back(std::make_unique<CAfficheurTexte>(pDispositif, 100, 100, FontContext::get()));
}
void CEndgame_Menu::afficher()
{
    renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(2).get(), fond_men));
    renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(0).get(), afficheurs.at(0)->GetTextureView()));
    renderer->push_hud_command(RenderHudObjectCommand(billboard_texte.at(1).get(), afficheurs.at(1)->GetTextureView()));
    afficheurs.at(0)->Ecrire(text_options.at(0), *pFont);
    afficheurs.at(1)->Ecrire(text_options.at(1), *pFont);
}
void CEndgame_Menu::gerer_input(const PM3D::CDIManipulateur& inp)
{
    pastTime = std::chrono::system_clock::now();
    if ((pastTime - currentTime) > std::chrono::milliseconds(75))
    {
        if (inp.ToucheAppuyee(KeyboardKey::enter))
            EventBus::instance().transmit(HUDEvent(HUDEvent::Event::MenuQuitter));
            
        currentTime = pastTime;
    }
}

void CEndgame_Menu::update_chrono(int min, int sec)
{
    if (!temps) {
        std::wstringstream stream;
        stream << std::setw(2) << std::setfill(L'0') << min
            << ":"
            << std::setw(2) << std::setfill(L'0') << sec;

        text_options.at(1) = stream.str();
        temps = true;
    }
}