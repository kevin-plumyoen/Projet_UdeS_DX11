#include "PetitMoteur3D.h"
#include "Moteur.h"
#include "AfficheurTexte.h"

#include "Win32Backend.h"
#include <tchar.h>

#include "Transform.h"

using namespace PM3D;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
    using Engine = Moteur<Game, Win32Backend>;
	// Création de l'objet Moteur
	Engine& rMoteur = Engine::GetInstance();

	// Spécifiques à une application Windows
    rMoteur.SetPlatformAppInstance(&hInstance);

	// Initialisation du moteur
	if(!rMoteur.Initialisations()) {
        // Initialization failed
	    return 1;
    }

    rMoteur.Run();

    return 0;
}
