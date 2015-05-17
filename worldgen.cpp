#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include "worldgen.h"
#include "xxhash.h"

void GenerateWorld(char* worldfile){
  tag_tiles tiles[num_zones];
  FILE *filein = fopen(worldfile, "wb");

  int seed;
  std::cout << "Enter seed:";
  std::cin >> seed;
  std::cout << GetHash(seed) << "\n";
  srand(seed);

  int percent;
  for (int i = 0; i < size_zone; i++){
    for (int j = 0; j < size_zone; j++){
      //tiles[0].tile_id [i] [j] = rand() % 2;
      percent = 1 + (rand() % 100);
      tiles[0].tile_id [i] [j] = 0;
      // 10 % chance of stone
      if (percent > 90){
        tiles[0].tile_id [i] [j] = 1;
      }
      // 20 % chance of water
      if ((percent > 70) && (percent < 90)){
        tiles[0].tile_id [i] [j] = 2;
      }
    }
  }
  int str_write;
  if (filein != NULL){
    str_write = fwrite(&tiles, sizeof(tiles), num_zones, filein);
  }
  else{
    std::cout << "Error writing file: " << worldfile << "!\n";
  }
  fclose(filein);
  return;
}

tag_tiles LoadWorld(char* worldfile){
  tag_tiles get_tiles[num_zones];
  FILE *filein = fopen(worldfile, "rb");
  if (filein != NULL){
    fread(&get_tiles, sizeof(get_tiles), num_zones, filein);
  }
  else{
    std::cout << "Error reading file: " << worldfile << "!\n";
  }
  fclose(filein);
  return(get_tiles[0]);
}
