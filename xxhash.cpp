#include "xxhash.h"

int seed;

const int PRIME32_1 = 2654435761U;
const int PRIME32_2 = 2246822519U;
const int PRIME32_3 = 3266489917U;
const int PRIME32_4 = 668265263U;
const int PRIME32_5 = 374761393U;

int RotateLeft (int value, int count){
  return (value << count) | (value >> (32 - count));
}

int GetHash (int buf){
  int h32 = (int)seed + PRIME32_5;
  h32 += 4U;
  h32 += (int)buf * PRIME32_3;
  h32 = RotateLeft (h32, 17) * PRIME32_4;
  h32 ^= h32 >> 15;
  h32 *= PRIME32_2;
  h32 ^= h32 >> 13;
  h32 *= PRIME32_3;
  h32 ^= h32 >> 16;
  return h32;
}
