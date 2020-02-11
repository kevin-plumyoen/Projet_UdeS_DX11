#include "pgm.h"

#include <string>

namespace ng {

PgmImage::pixel_type PgmImage::pixel_at(dimension_type x, dimension_type y) const {
    if (x >= width || y >= height) {
        throw std::out_of_range{"specified image coordinate is not inside bounds"};
    }

    return pixels[y * width + x];
}

PgmImage::pixel_type& PgmImage::pixel_at(dimension_type x, dimension_type y) {
    if (x >= width || y >= height) {
        throw std::out_of_range{"specified image coordinate is not inside bounds"};
    }

    return pixels[y * width + x];
}

std::istream& operator>>(std::istream& stream, PgmImage& pgm) {
    std::string magic;
    stream >> magic;

    if (!stream || magic != "P2") {
        stream.setstate(std::ios::failbit);
        return stream;
    }

    int width, height;
    stream >> width >> height;
    if (!stream) {
        return stream;
    }

    uint16_t max_value;
    stream >> max_value;
    if (!stream) {
        return stream;
    }

    const std::size_t total_size = width * height;

    std::vector<float> heights;
    heights.reserve(total_size);
    for (std::size_t i = 0; i < total_size; ++i) {
        uint16_t byte;
        if (!(stream >> byte)) {
            return stream;
        }
        else if (byte > max_value) {
            stream.setstate(std::ios::failbit);
            return stream;
        }

        const float normalized_height = static_cast<float>(byte) / static_cast<float>(max_value);
        heights.push_back(normalized_height);
    }

    pgm.width = width;
    pgm.height = height;
    pgm.pixels = std::move(heights);

    return stream;
}

}