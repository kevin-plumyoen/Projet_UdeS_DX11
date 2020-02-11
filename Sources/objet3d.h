#pragma once
#include "dispositif.h"
#include "Clock.h"
#include "HitBox.h"

class Camera;

namespace PM3D
{

//  Classe : CObjet3D
//
//  BUT : 	Classe de base de tous nos objets 3D
//
class CObjet3D
{
public:
	virtual ~CObjet3D() = default;

	virtual void Draw() = 0;
    virtual HitBox BoundingBox() const noexcept = 0;
};

} // namespace PM3D
