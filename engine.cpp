/*Basic c library*/
#include <iostream>

/*Open GL*/
#include "GL/glut.h"
#include "IL/il.h"
#include "IL/ilu.h"

/*My library*/
#include "engine.h"

// calculating left or up border current chunk or biome zone
// and compare it to next coordinate
bool CalculateBorder(int pos, unsigned short int zone, bool increase){
  // calculating left or up border current chunk or biome zone
  int border;
  border = pos + (zone / 2);
  if (border > 0){
    border = (((int)((border - 1) / zone)) * zone) + (zone / 2);
  }
  else{
    border = ((((int)(border / zone)) * zone) - (zone / 2));
  }
  // compare border and next coordinate
  if (increase){
    if ((pos + 1) == (border + 1)) return true;
    else return false;
  }
  else{
    if ((pos - 1) == (border - zone)) return true;
    else return false;
  }
}

