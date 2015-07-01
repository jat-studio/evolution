#include <GL/glut.h>

#include <iostream>

// calculating left or up border current chunk or biome zone
int CalculateBorder(int pos, unsigned short int zone){
  int border;
  border = pos + (zone / 2);
  if (border > 0){
    border = (((int)((border - 1) / zone)) * zone) + (zone / 2);
  }
  else{
    border = ((((int)(border / zone)) * zone) - (zone / 2));
  }
  return border;
}

