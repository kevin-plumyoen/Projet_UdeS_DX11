#include "Scene.h"
#include "Frustum.h"

using namespace std;

Scene::Scene(EntityRegistry* entities_) {
   Zone zonePersistente = Zone(Vec3(512, 512, 0.0), vector<Entity>(), 512);
   Zone zone1 = Zone(Vec3(256, 256, 0.0), vector<Entity>(), 256);
   Zone zone2 = Zone(Vec3(768, 256, 0.0), vector<Entity>(), 256);
   Zone zone3 = Zone(Vec3(256, 768, 0.0), vector<Entity>(), 256);
   Zone zone4 = Zone(Vec3(768, 768, 0.0), vector<Entity>(), 256);
   

   zones = { zonePersistente, zone1, zone2, zone3, zone4 };
   entities = entities_;
}

void Scene::activation(const Frustum& frustum) {
   for (Zone& zone : zones) {
      zone.activate(frustum);
   }
}

vector<Zone> Scene::activatedZones() const {
   vector<Zone> res;
   for (Zone zone : zones) {
      if (zone.isActivated()) {
         res.push_back(zone);
      }
   }
   return res;
}