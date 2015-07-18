using namespace std;
/*Basic c library*/
#include <cstdlib>
#include <iostream>
#include <array>

/*My library*/
#include "worldgen.h"

// string >> char_codes >> string
string StrToInt(string str){
  unsigned short int length = str.length();
  char buffer[3];
  string str_out;
  for (unsigned short int i = 0; i < length; i++){
    sprintf(buffer,"%d", (int)str[i]);
    str_out += buffer;
  }
  return str_out;
}

// function for calculating hash
unsigned int RotateLeft(unsigned int value, int count){
  return (value << count) | (value >> (32 - count));
}

// function for calculating hash
unsigned int CalcSubHash(unsigned int value, string buf, int index){
  unsigned int read_value = stoi(buf.substr(index, 4));
  value += read_value * PRIME32_2;
  value = RotateLeft(value, 13);
  value *= PRIME32_1;
  return value;
}

// function for calculating hash
unsigned int GetHash(string input){
  unsigned int seed = 0;
  unsigned int h32;
  unsigned short int index = 0;
  unsigned short int len = input.length();
  if (len >= 16){
    unsigned short int limit = len - 16;
	unsigned int v1 = seed + PRIME32_1 + PRIME32_2;
	unsigned int v2 = seed + PRIME32_2;
	unsigned int v3 = seed + 0;
	unsigned int v4 = seed - PRIME32_1;
	do{
      v1 = CalcSubHash(v1, input, index);
	  index += 4;
	  v2 = CalcSubHash(v2, input, index);
	  index += 4;
	  v3 = CalcSubHash(v3, input, index);
	  index += 4;
	  v4 = CalcSubHash(v4, input, index);
	  index += 4;
	}while (index <= limit);
	h32 = RotateLeft(v1, 1) + RotateLeft(v2, 7) + RotateLeft(v3, 12) + RotateLeft(v4, 18);
  }
  else{
    h32 = seed + PRIME32_5;
  }
  h32 += (unsigned int)len;
  while (index <= len - 4){
	h32 += stoi(input.substr(index, 4)) * PRIME32_3;
	h32 = RotateLeft (h32, 17) * PRIME32_4;
	index += 4;
  }
  while (index<len){
    h32 += stoi(input.substr(index, 1)) * PRIME32_5;
	h32 = RotateLeft (h32, 11) * PRIME32_1;
	index++;
  }

  h32 ^= h32 >> 15;
  h32 *= PRIME32_2;
  h32 ^= h32 >> 13;
  h32 *= PRIME32_3;
  h32 ^= h32 >> 16;

  return h32;
}

// generating ID Biomes
tag_biomes LoadBiomes(string seed, string coords){
  tag_biomes biomes_temp[1];
  unsigned short int percent;

  seed += coords;
  seed += "43";
  srand(GetHash(seed));

  for (unsigned short int i = 0; i < size_zone_biomes; i++){
      for (unsigned short int j = 0; j < size_zone_biomes; j++){
        percent = 1 + (rand() % 100);
        biomes_temp[0].biome_id [i] [j] = 0;
        // 40 % chance
        if (percent > 60){
          biomes_temp[0].biome_id [i] [j] = 1;
        }
      }
    }
  return biomes_temp[0];
}

// function for generate field started on left down point (x , y) and size of (xsize, ysize) going to right and up
tag_tiles GenerateField(unsigned short int x, unsigned short int y, unsigned short int xsize, unsigned short int ysize, unsigned short int id_biome){
  tag_tiles tiles_temp[1];
  unsigned short int percent;
  for (unsigned short int i = x; i < xsize; i++){
      for (unsigned short int j = y; j < ysize; j++){
        percent = 1 + (rand() % 100);
        tiles_temp[0].tile_id [i] [j] = 0;
        // 10 % chance
        if (percent > 90){
          tiles_temp[0].tile_id [i] [j] = 1;
        }
        if (id_biome == 1){
          tiles_temp[0].tile_id [i] [j] += 2;
        }
      }
    }
  return tiles_temp[0];
}

// function for generate current chunk
tag_tiles LoadChunk(string seed, string coords, unsigned short int id_biome){
  tag_tiles tiles[1];

  // size zone for calculating collision of chunks with different biomes
  /*unsigned short int size_collide = 1;
  if ((size_zone / 4) > 1) size_collide = size_zone / 4;*/

  // generate current chunk
  seed += coords;
  srand(GetHash(seed));
  tiles[0] = GenerateField(0, 0, size_zone, size_zone, id_biome);
  /*// generate center part of chunk
  tiles[0] = GenerateField();
  // generate up part of chunk
  tiles[0] = GenerateField();
  // generate up-right part of chunk
  tiles[0] = GenerateField();
  // generate right part of chunk
  tiles[0] = GenerateField();
  // generate down-right part of chunk
  tiles[0] = GenerateField();
  // generate down part of chunk
  tiles[0] = GenerateField();
  // generate down-left part of chunk
  tiles[0] = GenerateField();
  // generate left part of chunk
  tiles[0] = GenerateField();
  // generate up-left part of chunk
  tiles[0] = GenerateField();*/

  return tiles[0];
}
