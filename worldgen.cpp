#include <stdio.h>
#include <cstdlib>

#include "worldgen.h"

void GenerateWorld(char* worldfile){
  tag_tiles tiles[num_zones];
  FILE *filein = fopen(worldfile, "wb");
  for (int i = 0; i < size_zone; i++){
    for (int j = 0; j < size_zone; j++){
      tiles[0].tile_id [i] [j] = rand() % 2;
    }
  }
  int str_write;
  if (filein != NULL){
    str_write = fwrite(&tiles, sizeof(tiles), num_zones, filein);
  }
  else{
    printf("error writing file: ", worldfile, "!\n");
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
    printf("error reading file: ", worldfile, "!\n");
  }
  fclose(filein);
  return(get_tiles[0]);
}
