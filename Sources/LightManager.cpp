#include "LightManager.h"
#include "MathHelpers.h"

void LightManager::clearLights() {
  point_lights.clear();
  dir_lights.clear();
  spot_lights.clear();
}

void LightManager::addPointLight(Shader::ShadersPointLight light) {
  point_lights.push_back(light);
}
void LightManager::addDirLight(Shader::ShadersDirLight light) {
  dir_lights.push_back(light);
}
void LightManager::addSpotLight(Shader::ShadersSpotLight light) {
  spot_lights.push_back(light);
}

std::pair<Shader::ShadersLightsParams, Shader::ShadersLightCount> LightManager::getLightsParams(Vec3) {
  using namespace DirectX;

  Shader::ShadersLightsParams lp = Shader::ShadersLightsParams{};

  int p = 0;
  int d = 0;
  int s = 0;

  for (auto light : point_lights) {
    lp.point_lights[p++] = light;
  }
  for (auto light : dir_lights) {
    lp.dir_lights[d++] = light;
  }
  for (auto light : spot_lights) {
    lp.spot_lights[s++] = light;
  }

  Shader::ShadersLightCount lc;
  lc.PointLightsCount = p;
  lc.DirLightsCount = d;
  lc.SpotLightsCount = s;

  return {lp,lc};
}