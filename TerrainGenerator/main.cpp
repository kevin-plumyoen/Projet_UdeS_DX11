#include "pgm.h"

#include <terrain_patch.h>

#include <iostream>
#include <fstream>
#include <optional>
#include <random>
#include <string>
#include <cassert>
#include <algorithm>

std::optional<ng::TerrainPatch> build_terrain_from_pgm(std::istream& stream, const float terrain_height, const float tile_size);
std::vector<ng::TerrainPatch> build_terrains_from_pgm(std::istream& stream, const float terrain_height, const float tile_size);

ng::TerrainPatch generate_terrain(ng::TerrainPatch::dimension_type width, ng::TerrainPatch::dimension_type depth, const float terrain_height, const float tile_size, uint32_t seed);

void show_cli_usage(std::ostream& out);

struct Arguments {
    std::string input_file;
    std::string output_file;
    float terrain_height;
    float tile_size;
    ng::TerrainPatch::dimension_type width;
    ng::TerrainPatch::dimension_type height;
    uint32_t seed;
    bool split_into_4;

    Arguments()
    : input_file{}
    , output_file{ "out.patch" }
    , terrain_height{ 1.f }
    , tile_size{ 1.f }
    , width{ 256 }
    , height{ 256 }
    , seed{std::random_device{}()}
    , split_into_4{false} {

    }

    static Arguments parse(int argc, char* argv[]) {
        try {
            Arguments args;

            for (int i = 1; i < argc; ++i) {
                const std::string argument(argv[i]);
                const std::string next_argument = i < argc - 1 ? argv[i + 1] : "";

                if(argument[0] == '/') {
                    if (argument == "/help") {
                        show_cli_usage(std::cout);
                        exit(0);
                    }
                    else if (argument == "/out" && i < argc - 1) {
                        args.output_file = next_argument;
                        ++i;
                    }
                    else if (argument == "/seed" && i < argc - 1) {
                        ++i;
                    }
                    else if (argument == "/width" && i < argc - 1) {
                        args.width = std::stoi(next_argument.c_str());
                        ++i;
                    }
                    else if (argument == "/depth" && i < argc - 1) {
                        args.height = std::stoi(next_argument.c_str());
                        ++i;
                    }
                    else if (argument == "/height" && i < argc - 1) {
                        args.terrain_height = std::strtof(next_argument.c_str(), 0);
                        ++i;
                    }
                    else if (argument == "/tile_size" && i < argc - 1) {
                        args.tile_size = std::strtof(next_argument.c_str(), 0);
                        ++i;
                    }
                    else if (argument == "/4") {
                        args.split_into_4 = true;
                    }
                    // Unknown option
                    else {
                        show_cli_usage(std::cout);
                        exit(1);
                    }
                }
                else {
                    args.input_file = argument;
                }
            }

            return args;
        }
        catch (...) {
            std::cerr << "failed to parse CLI arguments" << std::endl;
            show_cli_usage(std::cout);
            exit(1);
        }
    }
};

int generate_heightmap(const Arguments& args) {
    auto terrain = generate_terrain(args.width, args.height, args.terrain_height, args.tile_size, args.seed);
    
    std::ofstream out_stream(args.output_file, std::ios::binary);
    if (terrain.save(out_stream)) {
        std::cout << "heighmap saved" << std::endl;
        return 0;
    }
    else {
        std::cerr << "failed to save terrain patch to binary file" << std::endl;
    }

    return 1;
}

int load_heightmap(const Arguments& args) {
    std::ifstream input_stream(args.input_file);

    if (args.split_into_4) {
        std::cout << "spliting texture into 4 patches..." << std::endl;
        auto terrain_results = build_terrains_from_pgm(input_stream, args.terrain_height, args.tile_size);
        bool succeed = true;
        if (!terrain_results.empty()) {
            for (int patch_index = 0; patch_index < terrain_results.size(); ++patch_index) {
                std::cout << "saving patch into \"" << args.output_file << "\\" << patch_index << ".patch\"" << std::endl;

                std::ofstream out_stream(args.output_file + "\\" + std::to_string(patch_index) + ".patch", std::ios::binary);
                if (!terrain_results[patch_index].save(out_stream)) {
                    std::cerr << "  failed to save patch" << std::endl;
                    succeed = false;
                }
            }

            if (succeed) {
                std::cout << "terrain patches saved" << std::endl;
            }

            return succeed ? 0 : 1;
        }
        else {
            std::cerr << "failed to load terrain's PGM texture" << std::endl;
        }
    }
    else {
        auto terrain_result = build_terrain_from_pgm(input_stream, args.terrain_height, args.tile_size);

        if (terrain_result) {
            const ng::TerrainPatch& terrain = *terrain_result;

            std::ofstream out_stream(args.output_file, std::ios::binary);
            if (terrain.save(out_stream)) {
                std::cout << "heighmap saved" << std::endl;
                return 0;
            }
            else {
                std::cerr << "failed to save terrain patch to binary file" << std::endl;
            }
        }
        else {
            std::cerr << "failed to load terrain patch" << std::endl;
        }
    }

    return 1;
}

int main(int argc, char* argv[]) {
    Arguments args = Arguments::parse(argc, argv);

    if (args.input_file.empty()) {
        return generate_heightmap(args);
    }
    else {
        return load_heightmap(args);
    }
}

ng::TerrainPatch build_terrain_from_pgm_region(const ng::PgmImage& texture, const int min_x, const int min_y, const int max_x, const int max_y, const float terrain_height, const float tile_size) {
    assert(max_x > min_x);
    assert(max_y > min_y);

    const int width = max_x - min_x;
    const int height = max_y - min_y;

    ng::TerrainPatchBuilder builder(width, height, tile_size);
    for (uint32_t y = min_y; y < max_y; ++y) {
        const uint32_t relative_y = y - min_y;

        for (uint32_t x = min_x; x < max_x; ++x) {
            const uint32_t relative_x = x - min_x;

            builder.add_vertex(ng::TerrainPatch::vertex_type{relative_x * tile_size, 
                                                             relative_y * tile_size, 
                                                             texture.pixel_at(std::min(x, static_cast<uint32_t>(max_x - 1)), std::min(y, static_cast<uint32_t>(max_y - 1))) * terrain_height});
        }
    }

    return builder.build();
}

std::vector<ng::TerrainPatch> build_terrains_from_pgm(std::istream& stream, const float terrain_height, const float tile_size) {
    std::vector<ng::TerrainPatch> patches;

    ng::PgmImage texture;
    if (stream >> texture) {
        const uint32_t width = texture.width;
        const uint32_t height = texture.height;
        const uint32_t half_width = width / 2;
        const uint32_t half_height = height / 2;

        patches.push_back(build_terrain_from_pgm_region(texture, 0,              0,               half_width,  half_height, terrain_height, tile_size));
        patches.push_back(build_terrain_from_pgm_region(texture, half_width - 1, 0,               width - 1,   half_height, terrain_height, tile_size));
        patches.push_back(build_terrain_from_pgm_region(texture, 0,              half_height - 1, half_width,  height - 1,          terrain_height, tile_size));
        patches.push_back(build_terrain_from_pgm_region(texture, half_width - 1, half_height - 1, width - 1,   height - 1,          terrain_height, tile_size));
    }

    return patches;
}

std::optional<ng::TerrainPatch> build_terrain_from_pgm(std::istream& stream, const float terrain_height, const float tile_size) {
    // Load the Pgm texture
    ng::PgmImage texture;
    if (stream >> texture) {
        return build_terrain_from_pgm_region(texture, 
                                             0, 0, texture.width, texture.height, 
                                             tile_size, terrain_height);
    }

    return {};
}

ng::TerrainPatch generate_terrain(ng::TerrainPatch::dimension_type width, ng::TerrainPatch::dimension_type depth, const float terrain_height, const float tile_size, uint32_t seed) {
    std::mt19937 prng{seed};
    std::uniform_real_distribution<float> height_distrib(0.f, 1.f);

    // Generates a spiky terrain patch
    ng::TerrainPatchBuilder builder(width, depth, tile_size);
    for (uint32_t y = 0; y < depth; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            builder.add_vertex(ng::TerrainPatch::vertex_type{x * tile_size, y * tile_size, height_distrib(prng) * terrain_height});
        }
    }
    
    return builder.build();
}

void show_cli_usage(std::ostream& out) {
    out << "rush [/out <output_file>] [/width <width>] [/depth <depth>] [/height <height>] [/tile_size <tile_size>] [<input_file>]";
    out << "\n\n";

    out << "EXAMPLES:\n";
    out << "Generate a random map of 256x256 tiles\n";
    out << "  rush /width 256 /height 256 /tile_size 5.0 /out generated.patch\n";
    out << "Convert an image to a terrain patch\n";
    out << "  rush /out converted.patch /tile_size 5.0 /height 64.0 heightmap.pgm\n";
    out << "\n\n";

    out << "OPTIONS:\n";
    out << " /out          The path to the directory where to generate the patch files\n";
    out << " /seed         The seed to use when generating a heightmap\n";
    out << " /with         The number of tiles on the x axis\n";
    out << " /depth        The number of tiles on the y axis\n";
    out << " /height       The maximum height on the z axis\n";
    out << " /tile_size    The size of a tile in units\n";
    out << " /4            Divide input texture into 4 patches\n";
    out << std::flush;
}