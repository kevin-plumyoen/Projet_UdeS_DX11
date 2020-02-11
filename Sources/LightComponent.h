#ifndef DEF_ECS_LIGHT_COMPONENT_H
#define DEF_ECS_LIGHT_COMPONENT_H

#include "Couleur.h"
#include "MathHelpers.h"
#include "Shader.h"
#include <variant>

struct PointLight {
    float range;
};

struct DirectionalLight {
    Vec3 direction;
};

struct SpotLight {
    float angle;
    float range;
    Vec3 direction;
};

struct LightComponent {
    using LightVariant = std::variant<PointLight, DirectionalLight, SpotLight>;
    using StructVariant = std::variant<Shader::ShadersPointLight, Shader::ShadersDirLight, Shader::ShadersSpotLight>;
    LightVariant light;
    Couleur diffuseColor;
    Couleur specularColor;
    float intensity;
    
    Vec3 offset = {0.f,0.f,0.f};

    StructVariant getParams() const {

        auto &diffuseColor_proxy = diffuseColor;
        auto &specularColor_proxy = specularColor;
        auto &intensity_proxy = intensity;
        
        StructVariant sv = std::visit([&diffuseColor_proxy, &specularColor_proxy, &intensity_proxy](const auto& light) {
          using light_type = std::decay_t<decltype(light)>;

          StructVariant params;

          if constexpr(std::is_same_v<light_type, PointLight>) {
            using struct_type = Shader::ShadersPointLight;

            struct_type shader_struct;
            shader_struct.diffuseColor = diffuseColor_proxy;
            shader_struct.specularColor = specularColor_proxy;
            shader_struct.intensity = intensity_proxy;

            shader_struct.range = light.range;

            params = shader_struct;

            return params;
          }
          else if constexpr(std::is_same_v<light_type, DirectionalLight>) {
            using struct_type = Shader::ShadersDirLight;

            struct_type shader_struct;
            shader_struct.diffuseColor = diffuseColor_proxy;
            shader_struct.specularColor = specularColor_proxy;
            shader_struct.intensity = intensity_proxy;

            shader_struct.dir = DirectX::XMVectorSet(light.direction.x, light.direction.y, light.direction.z, 1.0f);

            params = shader_struct;

            return params;
          }
          else if constexpr(std::is_same_v<light_type, SpotLight>) {
            using struct_type = Shader::ShadersSpotLight;

            struct_type shader_struct;
            shader_struct.diffuseColor = diffuseColor_proxy;
            shader_struct.specularColor = specularColor_proxy;
            shader_struct.intensity = intensity_proxy;

            shader_struct.range = light.range;
            shader_struct.angle = light.angle;
            shader_struct.dir = DirectX::XMVectorSet(light.direction.x, light.direction.y, light.direction.z, 1.0f);

            params = shader_struct;

            return params;
          }
          else {
            static_assert(false, "Type de lumière non-supportée");
          }
        }, light);;

        return sv;
    }
    
    template<typename T>
    bool is_type() const noexcept {
        return std::get_if<T>(&light) != nullptr;
    }
};

#endif