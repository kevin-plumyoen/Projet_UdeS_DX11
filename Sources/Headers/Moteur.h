#pragma once
#include "Singleton.h"
#include "dispositif.h"
#include "Camera.h"
#include "LevelCamera.h"
#include "FreeCamera.h"

#include <vector>
#include "Objet3D.h"
#include "Bloc.h"
#include "BlocEffet1.h"
#include "TerrainPatchEffet.h"
#include "terrain_patch.h"

#include "DIManipulateur.h"

#include <fstream>

namespace PM3D
{

const int IMAGESPARSECONDE = 60;
const float EcartTemps = 1.0f / static_cast<float>(IMAGESPARSECONDE);

//
//   TEMPLATE�: CMoteur
//
//   BUT�: Template servant � construire un objet Moteur qui implantera les
//         aspects "g�n�riques" du moteur de jeu
//
//   COMMENTAIRES�:
//
//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques 
//        du syst�me (ex: le moteur lui-m�me, le lien vers 
//        le dispositif Direct3D), l'utilisation d'un singleton 
//        nous simplifiera plusieurs aspects.
//
template <class T, class TClasseDispositif> class CMoteur :public CSingleton<T>
{
public:
    CDIManipulateur & GetGestionnaireDeSaisie() { return input; }
	virtual void Run()
	{
		bool bBoucle = true;

		while (bBoucle)
		{
			// Propre � la plateforme - (Conditions d'arr�t, interface, messages)
			bBoucle = RunSpecific();

			// appeler la fonction d'animation
			if (bBoucle)
			{
				bBoucle = Animation();
			}
		}
	}

	virtual int Initialisations()
	{
		// Propre � la plateforme
		InitialisationsSpecific();

		// * Initialisation du dispositif de rendu
		pDispositif = CreationDispositifSpecific(CDS_FENETRE);

		// * Initialisation de la sc�ne
		InitScene();

        InitTransformations();

		// * Initialisation des param�tres de l'animation et 
		//   pr�paration de la premi�re image
		InitAnimation();

		return 0;
	}

	virtual bool Animation()
	{
		// m�thode pour lire l'heure et calculer le 
		// temps �coul�
		const int64_t TempsCompteurCourant = GetTimeSpecific();
		const float TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

		// Est-il temps de rendre l'image?
		if (TempsEcoule > EcartTemps)
		{
      updateInputs();

      if (input.EtatSouris().rgbButtons[0] && currentCamera != 0) --currentCamera;
      else if (input.EtatSouris().rgbButtons[1] && currentCamera != cameras.size() - 1) ++currentCamera;

			// Affichage optimis�
			pDispositif->Present(); // On enlevera �//� plus tard

									// On pr�pare la prochaine image
			AnimeScene(TempsEcoule);

      cameras[currentCamera]->update(TempsEcoule);

			// On rend l'image sur la surface de travail
			// (tampon d'arri�re plan)
			RenderScene();

			// Calcul du temps du prochain affichage
			TempsCompteurPrecedent = TempsCompteurCourant;
		}

		return true;
	}

  void updateInputs() {
    input.StatutClavier();
    input.SaisirEtatSouris();
  }

	const XMMATRIX& GetMatView() const { return m_MatView; }
	const XMMATRIX& GetMatProj() const { return m_MatProj; }
	const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

protected:

	virtual ~CMoteur()
	{
		Cleanup();
	}

	// Sp�cifiques - Doivent �tre implant�s
	virtual bool RunSpecific() = 0;
	virtual int InitialisationsSpecific() = 0;

	virtual int64_t GetTimeSpecific() const = 0;
	virtual float GetTimeIntervalsInSec(int64_t start, int64_t stop) const = 0;

	virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
	virtual void BeginRenderSceneSpecific() = 0;
	virtual void EndRenderSceneSpecific() = 0;

	// Autres fonctions
	virtual int InitAnimation()
	{
		TempsSuivant = GetTimeSpecific();
		TempsCompteurPrecedent = TempsSuivant;

		// premi�re Image
		RenderScene();

		return true;
	}

	// Fonctions de rendu et de pr�sentation de la sc�ne
	virtual bool RenderScene()
	{
		BeginRenderSceneSpecific();

		// Appeler les fonctions de dessin de chaque objet de la sc�ne
		for (auto& object3D : ListeScene)
		{
			object3D->Draw(ActiveCamera());
		}

		EndRenderSceneSpecific();
		return true;
	}


	virtual void Cleanup()
	{
		// d�truire les objets
		ListeScene.clear();

		// D�truire le dispositif
		if (pDispositif)
		{
			delete pDispositif;
			pDispositif = nullptr;
		}
	}

  
  virtual int InitTransformations() {
    
    cameras.push_back(new FreeCamera(XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f),
      XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f),
      XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f),
      &m_MatView,
      &m_MatProj,
      &m_MatViewProj));

    cameras.push_back(new LevelCamera(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
      XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f),
      XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f),
      &m_MatView,
      &m_MatProj,
      &m_MatViewProj,
      ListeScene[0].get()));
    
    cameras[currentCamera]->update(0.0f);

    return 0;
  }
  
	virtual int InitScene()
	{
		// Initialisation des objets 3D - cr�ation et/ou chargement
		if (!InitObjets())
		{
			return 1;
		}

		// Initialisation des matrices View et Proj
		// Dans notre cas, ces matrices sont fixes
		/*m_MatView = XMMatrixLookAtLH(
			XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));*/

		const float champDeVision = XM_PI / 4; 	// 45 degr�s
		const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
		const float planRapproche = 2.0;
		const float planEloigne = 20.0;

		m_MatProj = XMMatrixPerspectiveFovLH(
			champDeVision,
			ratioDAspect,
			planRapproche,
			planEloigne);


		// Calcul de VP � l'avance
		//m_MatViewProj = m_MatView * m_MatProj;

		return 0;
	}

	bool InitObjets()
	{
        std::ifstream patch_file("..\\..\\Resources\\out.patch", std::ios::binary);
        ng::TerrainPatch patch;

        if (!patch.load(patch_file)) {
            std::cerr << "impossible de charger le terrain patch : " << std::endl;
            return false;
        }

		// Puis, il est ajout� � la sc�ne
        ListeScene.emplace_back(std::make_unique<CTerrainPatchEffet>(patch, pDispositif));
		//ListeScene.emplace_back(std::make_unique<CBlocEffet1>(2.0f, 2.0f, 2.0f, pDispositif));

		return true;
	}

	bool AnimeScene(float tempsEcoule)
	{
		for (auto& object3D : ListeScene)
		{
			object3D->Anime(tempsEcoule);
		}

		return true;
	}
public:
    const Camera& ActiveCamera() const {
        return *cameras[currentCamera];
    }
protected:
	// Variables pour le temps de l'animation
	int64_t TempsSuivant;
	int64_t TempsCompteurPrecedent;

	// Le dispositif de rendu
	TClasseDispositif* pDispositif;

	// La seule sc�ne
	std::vector<std::unique_ptr<CObjet3D>> ListeScene;

	// Les matrices
	XMMATRIX m_MatView;
	XMMATRIX m_MatProj;
	XMMATRIX m_MatViewProj;

    std::vector<Camera*> cameras;
    short currentCamera = 0;
    PM3D::CDIManipulateur input;
};

} // namespace PM3D
