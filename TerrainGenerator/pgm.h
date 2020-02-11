#ifndef DEF_PGM_H
#define DEF_PGM_H

#include <cstdint>
#include <vector>
#include <iostream>

namespace ng {

struct PgmImage {
    using dimension_type = uint32_t;
    using pixel_type = float;

    dimension_type width{};
    dimension_type height{};
    std::vector<pixel_type> pixels{};

    pixel_type pixel_at(dimension_type x, dimension_type y) const;
    pixel_type& pixel_at(dimension_type x, dimension_type y);
};

std::istream& operator>>(std::istream& stream, PgmImage& pgm);

}

#endif