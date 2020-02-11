#ifndef DEF_GAME_COLLISION_MASKS_H
#define DEF_GAME_COLLISION_MASKS_H

namespace CollisionMasks {
    enum : uint32_t {
        PlayerBit = 1,
        GrappableBit = 2,
        CameraObstacleBit = 4
    };
}

#endif