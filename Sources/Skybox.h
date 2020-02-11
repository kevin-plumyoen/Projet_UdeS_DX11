#pragma once

#include "CMeshImporter.h"

class Skybox : public PM3D::CMeshImporter {
public:
    Skybox(const Mesh& maScene, PM3D::CDispositifD3D11* pDispositif)
    : PM3D::CMeshImporter(maScene, pDispositif) {
    }
    void Draw() override;
};