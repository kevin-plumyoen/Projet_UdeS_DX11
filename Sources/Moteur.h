#pragma once
#include "Singleton.h"
#include "dispositif.h"
#include "dispositifD3D11.h"
#include "Clock.h"
#include "MathHelpers.h"
#include "DIManipulateur.h"
#include "GestionnaireDeTextures.h"
#include "Gestionnaires.h"
#include "PhysicEngine.h"
#include "Game.h"
#include "EngineContext.h"
#include "BackendTraits.h"
#include "Logger.h"
#include "EventBus.h"
#include "Profiler.h"
#include "AfficheurTexte.h"

#include <vector>
#include <fstream>
#include <chrono>
#include <memory>


namespace PM3D
{
    //
    //   TEMPLATE : CMoteur
    //
    //   BUT : Template servant à construire un objet Moteur qui implantera les
    //         aspects "génériques" du moteur de jeu
    //
    //   COMMENTAIRES :
    //
    //        Comme plusieurs de nos objets représenteront des éléments uniques 
    //        du système (ex: le moteur lui-même, le lien vers 
    //        le dispositif Direct3D), l'utilisation d'un singleton 
    //        nous simplifiera plusieurs aspects.
    //
    //template <class T, class TClasseDispositif> class CMoteur :public CSingleton<T>
    template<typename Game, typename Backend>
    class Moteur : public CSingleton<Moteur<Game, Backend>>
    {
    public:
        static_assert(is_backend_v<Backend>, "you must specify a valid engine backend type");
        using TClasseDispositif = typename backend_traits<Backend>::dispositif_type;
        using TInputs = typename backend_traits<Backend>::inputs_type;

        using clock = GameClock;
        using frame_duration = FrameDuration;
        using time_point = typename clock::time_point;

        static constexpr frame_duration TARGET_FRAME_DURATION{ std::chrono::microseconds(16670) };
    private:
        FontContext& fonts;
        Backend backend;
        std::unique_ptr<Game> game;

        time_point previous_time_point;

    protected:
        // Le dispositif de rendu
        TClasseDispositif * pDispositif;

        TInputs input;

        // Le gestionnaire de texture
        CGestionnaireDeTextures TexturesManager;
        GestionnaireMateriel MaterielsManager;
        GestionnaireShader ShadersManager;
        PhysicEngine physic;

    public:
        void SetPlatformAppInstance(void* platform_data) {
            backend.SetPlatformAppInstance(platform_data);
        }

        void Run()
        {
            bool bBoucle = true;

            while (bBoucle && !game->should_quit())
            {
                // Propre à la plateforme - (Conditions d'arrêt, interface, messages)
                bBoucle = backend.Run();

                // appeler la fonction d'animation
                if (bBoucle)
                {
                    if (backend.size_changed()) {
                        const auto new_size = backend.last_screen_size();
                        input.update_screen_size(new_size.first, new_size.second);
                        game->screen_size_changed(new_size.first, new_size.second);
                    }

                    bBoucle = Animation();
                }
            }
        }

        bool Initialisations()
        {
            Logger::instance().start();

            // Propre à la plateforme
            log_info("engine.init", "initialisation du backend...");
            backend.Initialisations(input);

            log_info("engine.init", "hide cursor...");
            input.hide_cursor();

            // * Initialisation du dispositif de rendu
            log_info("engine.init", "creation du dispositif...");
            pDispositif = backend.CreationDispositif(CDS_FENETRE);

            log_info("engine.init", "instanciation du jeu...");
            game = std::make_unique<Game>(EngineContext<TClasseDispositif,TInputs>(*pDispositif, TexturesManager, ShadersManager, MaterielsManager, input), physic);

            log_info("engine.init", "initialisation du jeu...");
            if (game->init() == false) return false;

            // * Initialisation des paramètres de l'animation et 
            //   préparation de la première image
            log_info("engine.init", "initialisation de la boucle de jeu...");
            InitAnimation();

            return true;
        }

        void GererEntreesUtilisateur(frame_duration dt) {
            updateInputs();

            game->handle_inputs(input, dt);
        }

        void AfficherTrame(frame_duration) {
            pDispositif->Present();

            RenderScene();
        }

        void UpdatePhysic(frame_duration dt) {
            if(game->is_paused()) return; // Disable physic update when game is paused

            physic.events().clear_triggered();
            game->pre_physic_update(dt);
            physic.simulate(dt);
            game->post_physic_update(dt);
        }

        bool Animation() {
            const time_point current_time = clock::now();
            const frame_duration frame_dt = current_time - previous_time_point;

            const uint32_t ms_target = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(TARGET_FRAME_DURATION).count());

            if (frame_dt > TARGET_FRAME_DURATION) {
                PROFILE_INSTANT_GLOBAL("start of frame", "engine");
                EventBus::instance().clear();

                AfficherTrame(frame_dt);

                game->start_of_frame(frame_dt);

                GererEntreesUtilisateur(frame_dt);
                
				game->update(frame_dt);
                
				UpdatePhysic(frame_dt);

                game->late_update(frame_dt);

                game->pre_render();

                game->end_of_frame(frame_dt);

                // TODO: Sleep

                previous_time_point = current_time;
            }

            return true;
        }

        void updateInputs() {
            input.StatutClavier();
            input.SaisirEtatSouris();
        }

        Moteur()
        : fonts{FontContext::get()} {

        }

        ~Moteur() {
            game->cleanup();

            input.show_cursor();

            delete pDispositif;
        }

    protected:
        // Autres fonctions
        int InitAnimation() {
            previous_time_point = clock::now();

            // première Image
            RenderScene();

            return true;
        }

        // Fonctions de rendu et de présentation de la scène
        bool RenderScene() {
            backend.BeginRenderScene(pDispositif);
            game->render();
            backend.EndRenderScene(pDispositif);

            return true;
        }
    };

} // namespace PM3D
