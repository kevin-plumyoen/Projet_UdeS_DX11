#ifndef DEF_IDENTITIES_H
#define DEF_IDENTITIES_H

namespace Identities {

enum : uint32_t {
	Goal   = (1 << 0),
    Tunnel = (1 << 1),
    Player = (1 << 2)
};

}
#endif