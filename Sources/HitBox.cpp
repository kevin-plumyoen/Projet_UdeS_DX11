#include "HitBox.h"
#include "Mesh.h"

#include <algorithm>

namespace PM3D {

HitBox::HitBox() noexcept
: centre{ 0.f, 0.f, 0.f }
, scaleBox{ 0.f, 0.f, 0.f } {

}

HitBox::HitBox(Vec3 centre, Vec3 scale) noexcept
: centre{ centre }, scaleBox{ scale } {
   hitBoxPoints[0] = Vec3{ centre.x + scaleBox.x ,centre.y + scaleBox.y, centre.z + scaleBox.z };
   hitBoxPoints[1] = Vec3{ centre.x + scaleBox.x ,centre.y + scaleBox.y, centre.z - scaleBox.z };
   hitBoxPoints[2] = Vec3{ centre.x + scaleBox.x ,centre.y - scaleBox.y, centre.z + scaleBox.z };
   hitBoxPoints[3] = Vec3{ centre.x + scaleBox.x ,centre.y - scaleBox.y, centre.z - scaleBox.z };
   hitBoxPoints[4] = Vec3{ centre.x - scaleBox.x ,centre.y + scaleBox.y, centre.z + scaleBox.z };
   hitBoxPoints[5] = Vec3{ centre.x - scaleBox.x ,centre.y + scaleBox.y, centre.z - scaleBox.z };
   hitBoxPoints[6] = Vec3{ centre.x - scaleBox.x ,centre.y - scaleBox.y, centre.z + scaleBox.z };
   hitBoxPoints[7] = Vec3{ centre.x - scaleBox.x ,centre.y - scaleBox.y, centre.z - scaleBox.z };
}

void HitBox::transform(Transform t) {
   for (Vec3& hitBoxPoint : hitBoxPoints) {
      hitBoxPoint = t.transform_point(hitBoxPoint);
   }
   *this = hitbox_from_Points();
   
}

HitBox HitBox::operator+(const HitBox& other) const noexcept {
    const Vec3 min_this = hitBoxPoints[7];
    const Vec3 max_this = hitBoxPoints[0];

    const Vec3 min_other = other.hitBoxPoints[7];
    const Vec3 max_other = other.hitBoxPoints[0];

    const float min_x = std::min(min_this.x, min_other.x);
    const float min_y = std::min(min_this.y, min_other.y);
    const float min_z = std::min(min_this.z, min_other.z);

    const float max_x = std::max(max_this.x, max_other.x);
    const float max_y = std::max(max_this.y, max_other.y);
    const float max_z = std::max(max_this.z, max_other.z);

    const float width = max_x - min_x;
    const float half_width = width / 2.f;
    const float height = max_y - min_y;
    const float half_height = height / 2.f;
    const float depth = max_z - min_z;
    const float half_depth = depth / 2.f;

    return HitBox(Vec3{min_x + half_width, min_y + half_height, min_z + half_depth}, 
                  Vec3{half_width, half_height, half_depth});
}

HitBox HitBox::hitbox_from_mesh(const Mesh& mi) noexcept {
    float xmin = std::numeric_limits<float>::max(), xmax = std::numeric_limits<float>::min();
    float ymin = std::numeric_limits<float>::max(), ymax = std::numeric_limits<float>::min();
    float zmin = std::numeric_limits<float>::max(), zmax = std::numeric_limits<float>::min();

    for (const Mesh::Vertex& sommet : mi.vertices) {
        xmin = std::min(xmin, sommet.position.x);
        xmax = std::max(xmax, sommet.position.x);
        ymin = std::min(ymin, sommet.position.y);
        ymax = std::max(ymax, sommet.position.y);
        zmin = std::min(zmin, sommet.position.z);
        zmax = std::max(zmax, sommet.position.z);
    }

    return HitBox(Vec3{ (xmax + xmin) / 2.0f, (ymax + ymin) / 2.0f, (zmax + zmin) / 2.0f },
                  Vec3{ (xmax - xmin) / 2.0f, (ymax - ymin) / 2.0f, (zmax - zmin) / 2.0f });
}

HitBox HitBox::hitbox_from_aiMesh(const aiMesh& mi) noexcept {
   float xmin = std::numeric_limits<float>::max(), xmax = std::numeric_limits<float>::min();
   float ymin = std::numeric_limits<float>::max(), ymax = std::numeric_limits<float>::min();
   float zmin = std::numeric_limits<float>::max(), zmax = std::numeric_limits<float>::min();

   std::vector<Vec3> vertex;
   vertex.reserve(mi.mNumVertices);

   std::for_each(mi.mVertices, mi.mVertices + mi.mNumVertices, [&vertex](const aiVector3D& vert) {
      vertex.push_back(Vec3(vert.x, vert.y, vert.z));
   });

   for (const auto& sommet : vertex) {
      xmin = std::min(xmin, sommet.x);
      xmax = std::max(xmax, sommet.x);
      ymin = std::min(ymin, sommet.y);
      ymax = std::max(ymax, sommet.y);
      zmin = std::min(zmin, sommet.z);
      zmax = std::max(zmax, sommet.z);
   }

   return HitBox(Vec3{ (xmax + xmin) / 2.0f, (ymax + ymin) / 2.0f, (zmax + zmin) / 2.0f },
                 Vec3{ (xmax - xmin) / 2.0f, (ymax - ymin) / 2.0f, (zmax - zmin) / 2.0f });
}

HitBox HitBox::hitbox_from_Points() noexcept {
   float xmin = std::numeric_limits<float>::max(), xmax = std::numeric_limits<float>::min();
   float ymin = std::numeric_limits<float>::max(), ymax = std::numeric_limits<float>::min();
   float zmin = std::numeric_limits<float>::max(), zmax = std::numeric_limits<float>::min();

   for (const Vec3& hitBoxPoint : hitBoxPoints) {
      xmin = std::min(xmin, hitBoxPoint.x);
      xmax = std::max(xmax, hitBoxPoint.x);
      ymin = std::min(ymin, hitBoxPoint.y);
      ymax = std::max(ymax, hitBoxPoint.y);
      zmin = std::min(zmin, hitBoxPoint.z);
      zmax = std::max(zmax, hitBoxPoint.z);
   }

   return HitBox(Vec3{ (xmax + xmin) / 2.0f, (ymax + ymin) / 2.0f, (zmax + zmin) / 2.0f },
                 Vec3{ (xmax - xmin) / 2.0f, (ymax - ymin) / 2.0f, (zmax - zmin) / 2.0f });

}

}