#ifndef HITBOX_H
#define HITBOX_H

#include "MathHelpers.h"
#include "Frustum.h"
#include "Transform.h"
#undef min
#undef max
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

struct Mesh;

namespace PM3D
{
   struct HitBox {
      Vec3 centre;
      Vec3 scaleBox;
      std::vector<Vec3> hitBoxPoints = std::vector<Vec3>(8, Vec3{});

      HitBox() noexcept;
      HitBox(Vec3 centre, Vec3 scale = Vec3(1.0f, 1.0f, 1.0f)) noexcept;

      void transform(Transform t);

      HitBox operator+(const HitBox& other) const noexcept;

      static HitBox hitbox_from_mesh(const Mesh& mi) noexcept;
      static HitBox hitbox_from_aiMesh(const aiMesh& mi) noexcept;
      HitBox hitbox_from_Points() noexcept;
   };
}

#endif