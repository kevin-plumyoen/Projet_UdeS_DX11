#include "Frustum.h"
#include "Camera.h"

#include <algorithm>
#include <vector>
#include "Profiler.h"
using namespace std;

Frustum::Frustum(const Camera &cam, float near, float far, float angle, float ratio) 
: near_(near)
, far_(far)
, alpha(angle / 2.f) 
, ratio_(ratio) {
   demi_grand_angle = atan(sqrt(2)*tan(alpha));
    updateFrustum(cam);
}

void Frustum::updateFrustum(const Camera &cam) {
    using namespace DirectX;

    camera_direction = cam.getDirection();
    camera_up = cam.getUp();
    camera_pos = cam.getPosition();

    const XMVECTOR dir = XMLoadFloat3(&camera_direction);
    const XMVECTOR up = XMLoadFloat3(&camera_up);
    const XMVECTOR pos = XMLoadFloat3(&camera_pos);

    const XMVECTOR centreNear = pos + dir * near_;
    const XMVECTOR centreFar = pos + dir * far_;
    const XMVECTOR yCam = XMVector3Normalize(XMVector3Cross(dir, up));

    const float tan_angle = tan(alpha);
    // Longueur/2 = far * tan_angle * ratio

    XMStoreFloat3(&n1, centreNear + tan_angle * near_ * (up + ratio_ * yCam));
    XMStoreFloat3(&n2, centreNear + tan_angle * near_ * (up - ratio_ * yCam));
    XMStoreFloat3(&n3, centreNear - tan_angle * near_ * (up + ratio_ * yCam));
    XMStoreFloat3(&n4, centreNear + tan_angle * near_ * (-up + ratio_ * yCam));

    XMStoreFloat3(&f1, centreFar + tan_angle * far_ * (up + ratio_ * yCam));
    XMStoreFloat3(&f2, centreFar + tan_angle * far_ * (up - ratio_ * yCam));
    XMStoreFloat3(&f3, centreFar - tan_angle * far_ * (up + ratio_ * yCam));
    XMStoreFloat3(&f4, centreFar + tan_angle * far_ * (-up + ratio_ * yCam));

    projection();
}

void Frustum::projection() {
    xmin = std::min({ n1.x, n2.x, n3.x, n4.x, f1.x, f2.x, f3.x, f4.x }); // xmin
    xmax = std::max({ n1.x, n2.x, n3.x, n4.x, f1.x, f2.x, f3.x, f4.x }); // xmax
    ymin = std::min({ n1.y, n2.y, n3.y, n4.y, f1.y, f2.y, f3.y, f4.y }); // ymin
    ymax = std::max({ n1.y, n2.y, n3.y, n4.y, f1.y, f2.y, f3.y, f4.y }); // ymax

    p1 = Vec2(xmin, ymax);
    p2 = Vec2(xmax, ymax);
    p3 = Vec2(xmax, ymin);
    p4 = Vec2(xmin, ymin);
}


bool Frustum::contains(const PM3D::HitBox& hitbox) const noexcept {
   PROFILE_SCOPE("contains", "frustum");
   using namespace DirectX;
   // On vérifie premièrement les points primordiaux pour potentiellement éviter l'échantillonage
   // Vérifier si le centre de la hitbox appartient au frustum
   if (contains(hitbox.centre)) {
      return true;
   }

   // Vérifier les points centraux des faces de notre hitbox
   if (contains(Vec3{ hitbox.centre.x + hitbox.scaleBox.x ,hitbox.centre.y, hitbox.centre.z })) {
      return true;
   }
   if (contains(Vec3{ hitbox.centre.x - hitbox.scaleBox.x ,hitbox.centre.y, hitbox.centre.z })) {
      return true;
   }
   if (contains(Vec3{ hitbox.centre.x, hitbox.centre.y + hitbox.scaleBox.y, hitbox.centre.z })) {
      return true;
   }
   if (contains(Vec3{ hitbox.centre.x, hitbox.centre.y - hitbox.scaleBox.y, hitbox.centre.z })) {
      return true;
   }
   if (contains(Vec3{ hitbox.centre.x, hitbox.centre.y , hitbox.centre.z + hitbox.scaleBox.z })) {
      return true;
   }
   if (contains(Vec3{ hitbox.centre.x, hitbox.centre.y , hitbox.centre.z - hitbox.scaleBox.z })) {
      return true;
   }

   // Vérifier si les points définissant notre hitbox intersecte le frustum ou sont tous derrière la caméra
   //bool behindCam = true;
   for (const Vec3& hitBoxPoint : hitbox.hitBoxPoints) {  
      if (contains(hitBoxPoint)) {
         return true;
      }
   }


   // Convertir les points du frustum en XMVECTOR
   XMVECTOR _n1 = XMLoadFloat3(&n1);
   XMVECTOR _n2 = XMLoadFloat3(&n2); 
   XMVECTOR _n3 = XMLoadFloat3(&n3);
   XMVECTOR _n4 = XMLoadFloat3(&n4);
   XMVECTOR _f1 = XMLoadFloat3(&f1);
   XMVECTOR _f2 = XMLoadFloat3(&f2);
   XMVECTOR _f3 = XMLoadFloat3(&f3);
   XMVECTOR _f4 = XMLoadFloat3(&f4);
   XMVECTOR cam = XMLoadFloat3(&camera_pos);
   
   // Creer les normales du frstum, dirigé vers son extérieur
   vector<XMVECTOR> normalesFrustum = {};
   normalesFrustum.push_back(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_n2 - _n1, _n4 - _n1)));
   normalesFrustum.push_back(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_f4 - _f1, _f2 - _f1)));
   normalesFrustum.push_back(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_f1 - cam, _f2 - cam)));
   normalesFrustum.push_back(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_f4 - cam, _f1 - cam)));
   normalesFrustum.push_back(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_f2 - cam, _f3 - cam)));
   normalesFrustum.push_back(DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_f3 - cam, _f4 - cam)));

   //Si tous les points englobant d'une hitbox sont derrières un plan du frustum, on ne l'affiche pas
   // On parcours les normales, représentant chacunes un plan du frustum
   int indexNormales = 0;
   for (XMVECTOR normaleFrustum : normalesFrustum) {
      bool outPlan = true;
      // on parcours tous les points défénissant notre hitbos, si ils sont tous du même coté externe au frustum par rapport au plan, on affiche pas l'objet associé
      for (const Vec3& hitBoxPoint : hitbox.hitBoxPoints) {
         if (indexNormales == 0) {
            if (XMVector3Dot(XMLoadFloat3(&hitBoxPoint) - _n1, normaleFrustum).vector4_f32[0] < 0.0f) {
               outPlan = false;
               break;
            }
         } else if (indexNormales == 1) {
            if (XMVector3Dot(XMLoadFloat3(&hitBoxPoint) - _f1, normaleFrustum).vector4_f32[0] < 0.0f) {
               outPlan = false;
               break;
            }
         }
         else {
            if (XMVector3Dot(XMLoadFloat3(&hitBoxPoint) - cam, normaleFrustum).vector4_f32[0] < 0.0f) {
               outPlan = false;
               break;
            }
         }
      }
      if (outPlan) {
         return false;
      }
      ++indexNormales;
   }

   return true;
}

bool Frustum::contains(const Vec3& point) const noexcept {
   using namespace DirectX;
   const XMVECTOR cam2p = XMLoadFloat3(&point) - XMLoadFloat3(&camera_pos);
   float norm = cam2p.vector4_f32[0] / XMVector3Normalize(cam2p).vector4_f32[0];
   
   float demi_angle = abs(XMVector3AngleBetweenVectors(cam2p, XMLoadFloat3(&camera_direction)).vector4_f32[0]);
   
   return (norm >= near_ / cos(demi_grand_angle) && norm <= far_ / cos(demi_grand_angle)) // norm dans le frstum
          && (demi_angle < demi_grand_angle); // angle dans le cone englobant le frustum
}

bool Frustum::behind(const Vec3& point) const noexcept {
   using namespace DirectX;
   const XMVECTOR cam2p = XMLoadFloat3(&point) - XMLoadFloat3(&camera_pos);
   return XMVector3Dot(cam2p, XMLoadFloat3(&camera_direction)).vector4_f32[0] < 0.0f;
}