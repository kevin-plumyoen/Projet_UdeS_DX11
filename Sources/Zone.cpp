#include "Headers\Zone.h"
#include "Camera.h"

#include <algorithm>
class Iterator;
using namespace std;

Zone::Zone(Vec3 centre, const std::vector<Entity>& entities, float demiLongueur, float demiLargeur) 
: centre(centre)
, entities(entities)
, demiLongueur(demiLongueur)
, demiLargeur(demiLargeur) {
   xmin = centre.x - demiLargeur;
   xmax = centre.x + demiLargeur;
   ymin = centre.y - demiLongueur;
   ymax = centre.y + demiLongueur;

	p1 = Vec3(xmin, ymax, centre.z);
	p2 = Vec3(xmax, ymax, centre.z);
	p3 = Vec3(xmax, ymin, centre.z);
	p4 = Vec3(xmin, ymin, centre.z);
}


void Zone::activate(Frustum frustum)
{

   activated = // Si frustrum < zone
               (xmin <= frustum.p1.x && frustum.p1.x <= xmax && ymin <= frustum.p1.y && frustum.p1.y <= ymax) ||
               (xmin <= frustum.p2.x && frustum.p2.x <= xmax && ymin <= frustum.p2.y && frustum.p2.y <= ymax) ||
               (xmin <= frustum.p3.x && frustum.p3.x <= xmax && ymin <= frustum.p3.y && frustum.p3.y <= ymax) ||
               (xmin <= frustum.p4.x && frustum.p4.x <= xmax && ymin <= frustum.p4.y && frustum.p4.y <= ymax) ||
               // Si zone < frustrum 
               (frustum.xmin <= p1.x && p1.x <= frustum.xmax && frustum.ymin <= p1.y && p1.y <= frustum.ymax) ||
			      (frustum.xmin <= p2.x && p2.x <= frustum.xmax && frustum.ymin <= p2.y && p2.y <= frustum.ymax) ||
				   (frustum.xmin <= p3.x && p3.x <= frustum.xmax && frustum.ymin <= p3.y && p3.y <= frustum.ymax) ||
				   (frustum.xmin <= p4.x && p4.x <= frustum.xmax && frustum.ymin <= p4.y && p4.y <= frustum.ymax);
}

vector<pair<Entity, LOD>> Zone::lod(Camera & cam, const TransformStorage& transforms) {
   using namespace DirectX;
   vector<pair<Entity, LOD>> res = {};
   const Vec3 camPos = cam.getPosition();
   const XMVECTOR posCamera = XMLoadFloat3(&camPos);
   // Parcourir les entites
   for (Entity e : entities) {
      // Savoir si elle a une position (TransformComponent)
      if (transforms.is_attached(e)) {
         auto transform_it = transforms.find(e);       
         XMVECTOR posEntity = XMLoadFloat3(&transform_it->second.world.translation);
         XMVECTOR vecCam2Entity = posEntity - posCamera;
         float distCam2Entity = DirectX::XMVector3Length(vecCam2Entity).vector4_f32[0];
         if (distCam2Entity <= distanceLOD) {
            res.push_back(pair(e, LOD::HIGH));
         }
         else {
            res.push_back(pair(e, LOD::LOW));
         }
      }
   }

   return res;
}