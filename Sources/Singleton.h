#pragma once

namespace PM3D
{
    //
	//   TEMPLATE : CSingleton
	//
	//   BUT : Template servant à construire un objet unique
	//
	//   COMMENTAIRES :
	//
	//        Comme plusieurs de nos objets représenteront des éléments uniques 
	//		  du système (ex: le moteur lui-même, le lien vers 
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