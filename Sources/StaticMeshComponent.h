#ifndef DEF_ECS_STATIC_MESH_COMPONENT_H
#define DEF_ECS_STATIC_MESH_COMPONENT_H

#include "objet3d.h"
#include "CMeshImporter.h"
#include "Materiel.h"
#include <memory>

struct StaticMeshComponent {
    std::shared_ptr<Materiel> material;
    std::shared_ptr<PM3D::CMeshImporter> mesh;
};

#endif