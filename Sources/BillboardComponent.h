#ifndef DEF_BILLBOARD_COMPONENT_H
#define DEF_BILLBOARD_COMPONENT_H

#include "Billboard.h"
#include "Materiel.h"
#include <memory>

enum class BillboardLinearOrientation : uint8_t {
    OnlyZ,
    LookAt
};

struct BillboardComponent {
    std::shared_ptr<Billboard> billboard;
    std::shared_ptr<Materiel> material;
    BillboardLinearOrientation orientation;
};

#endif