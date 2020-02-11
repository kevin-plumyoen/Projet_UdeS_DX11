#pragma once

#include "SystemInterface.h"
#include "Couleur.h"
#include "ComponentStorages.h"

class VariableColorUpdateSystem : public BaseSystem {

  const VariableColorStorage& variable_colors;
  LightStorage& lights;
  StaticMeshStorage& static_meshes;

  Couleur hsv2rgb() const
  {
    float      h, p, q, t, ff;
    long       i;
    Couleur    out;

    float s = 1.0f;
    float v = 1.0f;

    h = hue/(DirectX::XM_PI/3);
    i = (long)h;
    ff = h - i;
    p = v * (1.0f - s);
    q = v * (1.0f - (s * ff));
    t = v * (1.0f - (s * (1.0f - ff)));

    switch (i) {
    case 0:
      out = Couleur{ v,t,p,1.0f };
      break;
    case 1:
      out = Couleur{ q,v,p,1.0f };
      break;
    case 2:
      out = Couleur{ p,v,t,1.0f };
      break;
    case 3:
      out = Couleur{ p,q,v,1.0f };
      break;
    case 4:
      out = Couleur{ t,p,v,1.0f };
      break;
    case 5:
    default:
      out = Couleur{ v,p,q,1.0f };
      break;
    }
    return out;
  }
  float hue = 0.0f;
  float color_change_speed() {
    return 0.0005f; 
  }
public:
  VariableColorUpdateSystem(const VariableColorStorage& variable_colors, LightStorage& lights, StaticMeshStorage& static_meshes) : variable_colors{ variable_colors } ,lights { lights }, static_meshes{ static_meshes } {}
  void process(const EntityRegistry&, FrameDuration, ProcessState) override;
};