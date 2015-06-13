#ifndef WORLDGEN_H_INCLUDED
#define WORLDGEN_H_INCLUDED

// size of chunk
const unsigned short int size_zone = 64;
// size of loaded biomes
const unsigned short int size_zone_biomes = 64;
// count of drawing chunks
const unsigned short int num_zones = 9;
// scale of scene
const float scale = 0.07;

// constants for calculating hash
const unsigned int PRIME32_1 = 2654435761;
const unsigned int PRIME32_2 = 2246822519;
const unsigned int PRIME32_3 = 3266489917;
const unsigned int PRIME32_4 = 668265263;
const unsigned int PRIME32_5 = 374761393;

// structure for content of chunk
struct tag_tiles{
  unsigned short int tile_id [size_zone] [size_zone];
};

// structure for biomes
struct tag_biomes{
  unsigned short int biome_id [size_zone_biomes] [size_zone_biomes];
};

tag_biomes LoadBiomes(std::string seed, std::string coords);
tag_tiles LoadChunk(std::string seed, std::string coords, unsigned short int id_biome);

#endif
