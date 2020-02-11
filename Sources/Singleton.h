#pragma once

namespace PM3D
{
    //
	//   TEMPLATE�: CSingleton
	//
	//   BUT�: Template servant � construire un objet unique
	//
	//   COMMENTAIRES�:
	//
	//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques 
	//		  du syst�me (ex: le moteur lui-m�me, le lien vers 
	//        le dispositif Direct3D), l'utilisation d'un singleton 
	//        nous simplifiera plusieurs aspects.
	//

	template <class T> 
    class CSingleton {
	public:
        // Renvoie l'instance unique de la classe
        static T& GetInstance() {
            static T instance_;
            return instance_;
        }

	protected :
		CSingleton() = default;

        CSingleton(CSingleton&) = delete;
        CSingleton& operator =(CSingleton&) = delete;
	};
}