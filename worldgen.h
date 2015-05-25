#ifndef WORLDGEN_H_INCLUDED
#define WORLDGEN_H_INCLUDED

const unsigned short int size_zone = 64;
const unsigned short int num_zones = 9;
const float scale = 0.07;

const unsigned int PRIME32_1 = 2654435761;
const unsigned int PRIME32_2 = 2246822519;
const unsigned int PRIME32_3 = 3266489917;
const unsigned int PRIME32_4 = 668265263;
const unsigned int PRIME32_5 = 374761393;

struct tag_tiles{
  int tile_id [size_zone] [size_zone];
};

tag_tiles LoadChunk(std::string seed, std::string coords);

#endif
