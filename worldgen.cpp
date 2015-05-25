#include <cstdlib>
#include <iostream>

#include "worldgen.h"

unsigned int RotateLeft(unsigned int value, int count){
  return (value << count) | (value >> (32 - count));
}

unsigned int CalcSubHash(unsigned int value, std::string buf, int index){
  unsigned int read_value = stoi(buf.substr(index, 4));
  value += read_value * PRIME32_2;
  value = RotateLeft(value, 13);
  value *= PRIME32_1;
  return value;
}

unsigned int GetHash(std::string input){
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

tag_tiles LoadChunk(std::string seed, std::string coords){
  tag_tiles tiles[1];

  seed += coords;
  srand(GetHash(seed));

  int percent;
  for (unsigned short int i = 0; i < size_zone; i++){
    for (unsigned short int j = 0; j < size_zone; j++){
      percent = 1 + (rand() % 100);
      tiles[0].tile_id [i] [j] = 0;
      // 10 % chance of stone
      if (percent > 90){
        tiles[0].tile_id [i] [j] = 1;
      }
      // 1 % chance of water
      if ((percent > 88) && (percent < 90)){
        tiles[0].tile_id [i] [j] = 2;
      }
    }
  }
  return tiles[0];
}
