#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include "Shader.h"
#include "MathHelpers.h"
#include <vector>
#include <utility>

class LightManager {
  std::vector<Shader::ShadersPointLight> point_lights;
  std::vector<Shader::ShadersDirLight> dir_lights;
  std::vector<Shader::ShadersSpotLight> spot_lights;

public:
  void clearLights();

  void addPointLight(Shader::ShadersPointLight);
  void addDirLight(Shader::ShadersDirLight);
  void addSpotLight(Shader::ShadersSpotLight);

  std::pair<Shader::ShadersLightsParams, Shader::ShadersLightCount> getLightsParams(Vec3);
};

#endif
