#ifndef WORLDGEN_H_INCLUDED
#define WORLDGEN_H_INCLUDED

const int size_zone = 64;
const int num_zones = 1;

struct tag_tiles{
  int tile_id [size_zone] [size_zone];
};

void GenerateWorld(char* worldfile);
void LoadWorld(char* worldfile, tag_tiles get_tiles[num_zones]);

#endif
