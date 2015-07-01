#include <GL/glut.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>

#include "worldgen.h"
#include "engine.h"

unsigned short int wnd;            // id GLwindow

const unsigned short int count_tex = 5; // count of textures
GLuint tiles_tex[count_tex - 1];    // index texture of tiles



// fps drawing
int fps;
long t, dt;
std::string fps_str;

// parameters for load and generate world with seed
tag_biomes loaded_biomes[3];          // three biome zones (current vertical and horizontal next zone)
tag_biomes tmp_biomes[1];             // temporary structure for redefine loaded_biomes
tag_tiles loaded_tiles[num_zones];
std::string str_seed, int_seed;

// coordinates of camera
float xpos_cam = 0.0;
float ypos_cam = 0.0;

// coordinates of tiles
int xpos;
int ypos;

// structure for id chunks
struct tag_id_chunks{
  unsigned short int ida;
  unsigned short int idb;
  unsigned short int idc;
};
tag_id_chunks id_chunks;

// structure for int coordinates of biomes zones
struct tag_coords_biomes{
  int x;
  int y;
};
tag_coords_biomes coords_biomes[1];

// structure for int coordinates of chunks
struct tag_coords_chunks{
  int x;
  int y;
};
tag_coords_chunks coords_chunks[9];

ClassScene Scene;

// loading textures
void LoadTextures(){
  // create array of textures
  glGenTextures(count_tex, &tiles_tex[0]);

  // initializing il and ilu library
  ilInit();
  iluInit();

  // loading textures
  Scene.LoadTextureImage("grass.png", tiles_tex[0]);
  Scene.LoadTextureImage("stone.png", tiles_tex[1]);
  Scene.LoadTextureImage("water.png", tiles_tex[2]);
  Scene.LoadTextureImage("water_wave.png", tiles_tex[3]);
  Scene.LoadTextureImage("player.tga", tiles_tex[4]);

  // enabling textures
  glEnable(GL_TEXTURE_2D);
}

// deleting textures
void clearTextures(){
  glDeleteTextures(count_tex, &tiles_tex[0]);
}

//drawing chunk
void DrawChunk(unsigned short int index, int x, int y){
  unsigned short int loop_col, loop_row, tile_get;

  glPushMatrix();
  glTranslatef(x * size_zone, y * size_zone, 0.0);
  for (loop_col = 0; loop_col < size_zone; loop_col++){
    for (loop_row = 0; loop_row < size_zone; loop_row++){
      tile_get = loaded_tiles[index].tile_id [loop_col] [loop_row];

      glBindTexture(GL_TEXTURE_2D, tiles_tex[tile_get]);

      glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, -1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(((size_zone / 2) - loop_row), (((-1 * size_zone) / 2) + loop_col + 1), 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(((size_zone / 2) - loop_row - 1), (((-1 * size_zone) / 2) + loop_col + 1), 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(((size_zone / 2) - loop_row - 1), (((-1 * size_zone) / 2) + loop_col + 2), 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3f(((size_zone / 2) - loop_row), (((-1 * size_zone) / 2) + loop_col + 2), 0.0);
      glEnd();
    }
  }
  glPopMatrix();
}

//drawing player
void DrawPlayer(){
  glPushMatrix();
  glTranslatef(xpos, ypos, 0.0);
  glBindTexture(GL_TEXTURE_2D, tiles_tex[4]);
  glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 1.0, -0.01);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, -0.01);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 0.0, -0.01);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, -0.01);
  glEnd();
  glPopMatrix();
}

// function for getting id biome from array using on input coordinates of chunk
unsigned short int GetIDBiome(int x, int y){
  /*Необходимо выдавать на основе координат чанков порядковые номера в массивах*/

  // redefining coordinates of chunk to id current array variables
  short int coeff_x = x + (size_zone_biomes / 2) - 1;
  short int coeff_y = y + (size_zone_biomes / 2) - 1;
  // if coordinate x ranges in array loaded_biomes[0]
  if ((coeff_x >= 0) and (coeff_x < size_zone_biomes)){
    // if coordinate y ranges in array loaded_biomes[0]
    if ((coeff_y >= 0) and (coeff_y < size_zone_biomes)){
      x = coeff_x;
      y = coeff_y;
      return loaded_biomes[0].biome_id[x][y];
    }
    // if coordinate y has run out of array loaded_biomes[0]
    else{
      // redefining y coordinate of chunk to id next array
      if (coeff_y < 0){
        y = coeff_x + size_zone_biomes;
      }
      else{
        y = coeff_y - size_zone_biomes;
      }
      x = coeff_x;
      return loaded_biomes[2].biome_id[x][y];
    }
  }
  // if coordinate x has run out of array loaded_biomes[0]
  else{
    // redefining x coordinate of chunk to id next array
    if (coeff_x < 0){
      x = coeff_x + size_zone_biomes;
    }
    else{
      x = coeff_x - size_zone_biomes;
    }
    y = coeff_y;
    return loaded_biomes[1].biome_id[x][y];
  }
}

// loading three chunks
void LoadThreeChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks_A, tag_coords_chunks coords_chunks_B, tag_coords_chunks coords_chunks_C){
  loaded_tiles[id_chunks.ida] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_A.x) + std::to_string(coords_chunks_A.y)), GetIDBiome(coords_chunks_A.x, coords_chunks_A.y));
  loaded_tiles[id_chunks.idb] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_B.x) + std::to_string(coords_chunks_B.y)), GetIDBiome(coords_chunks_B.x, coords_chunks_B.y));
  loaded_tiles[id_chunks.idc] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_C.x) + std::to_string(coords_chunks_C.y)), GetIDBiome(coords_chunks_C.x, coords_chunks_C.y));
}

// moving and generate chunks
// parameters: horizontal moving or not and increase or decrease coordinates
void MoveChunks(bool horizontal, bool increase){
  // redefining chunk id and coordinates
  std::array<unsigned short int, 9> id;
  short int coeff = 0;
  if (horizontal){
    if (increase){
      // left
      id = {2, 3, 4, 1, 0, 5, 8, 7, 6};
      coeff = 1;
    }
    else{
      // right
      id = {8, 7, 6, 1, 0, 5, 2, 3, 4};
      coeff = -1;
    }
    coords_chunks[0].x += coeff;
    coords_chunks[1].x += coeff;
    coords_chunks[2].x += coeff;
    coords_chunks[3].x += coeff;
    coords_chunks[4].x += coeff;
    coords_chunks[5].x += coeff;
    coords_chunks[6].x += coeff;
    coords_chunks[7].x += coeff;
    coords_chunks[8].x += coeff;
  }
  else{
    if (increase){
      // up
      id = {6, 5, 4, 7, 0, 3, 8, 1, 2};
      coeff = 1;
    }
    else{
      // down
      id = {8, 1, 2, 7, 0, 3, 6, 5, 4};
      coeff = -1;
    }
    coords_chunks[0].y += coeff;
    coords_chunks[1].y += coeff;
    coords_chunks[2].y += coeff;
    coords_chunks[3].y += coeff;
    coords_chunks[4].y += coeff;
    coords_chunks[5].y += coeff;
    coords_chunks[6].y += coeff;
    coords_chunks[7].y += coeff;
    coords_chunks[8].y += coeff;
  }

  // redefining content of chunks
  loaded_tiles[id[0]] = loaded_tiles[id[3]];
  loaded_tiles[id[1]] = loaded_tiles[id[4]];
  loaded_tiles[id[2]] = loaded_tiles[id[5]];

  loaded_tiles[id[3]] = loaded_tiles[id[6]];
  loaded_tiles[id[4]] = loaded_tiles[id[7]];
  loaded_tiles[id[5]] = loaded_tiles[id[8]];

  // generate three new chunks
  LoadThreeChunks({id[6], id[7], id[8]},
                  {coords_chunks[id[6]].x, coords_chunks[id[6]].y},
                  {coords_chunks[id[7]].x, coords_chunks[id[7]].y},
                  {coords_chunks[id[8]].x, coords_chunks[id[8]].y});
}

// repainting OpenGL by reshape window
void Reshape(GLsizei Width, GLsizei Height){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glViewport(0, 0, Width, Height);
  gluPerspective(45.0, (GLfloat)Width/(GLfloat)Height, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

// painting Scene
void Draw(){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(xpos_cam, ypos_cam, -6.0,
            xpos_cam, ypos_cam, 1.0,
            0.0, 1.0, 0.0);

  // painting world
  glScalef(scale, scale, 0.0);

  DrawChunk(0, coords_chunks[0].x, coords_chunks[0].y);
  DrawChunk(1, coords_chunks[1].x, coords_chunks[1].y);
  DrawChunk(2, coords_chunks[2].x, coords_chunks[2].y);
  DrawChunk(3, coords_chunks[3].x, coords_chunks[3].y);
  DrawChunk(4, coords_chunks[4].x, coords_chunks[4].y);
  DrawChunk(5, coords_chunks[5].x, coords_chunks[5].y);
  DrawChunk(6, coords_chunks[6].x, coords_chunks[6].y);
  DrawChunk(7, coords_chunks[7].x, coords_chunks[7].y);
  DrawChunk(8, coords_chunks[8].x, coords_chunks[8].y);

  DrawPlayer();



  // painting text on 2d mode
  Scene.setOrthographicProjection();
  // drawing fps
  fps++;
  t = glutGet(GLUT_ELAPSED_TIME);
  if (t - dt > 1000){
    fps_str = "FPS:" + std::to_string((int)(fps * 1000.0 / (t - dt)));
    dt = t;
    fps = 0;
  }
  Scene.DrawStaticString(-0.99, 0.95, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, fps_str);

  // drawing coordinates
  std::string print_str;
  print_str = "X: ";
  print_str += std::to_string(xpos);
  print_str += "; Y: ";
  print_str += std::to_string(ypos);
  Scene.DrawStaticString(-0.99, 0.91, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str);

  // drawing coordinates chunks
  std::string print_str1;
  print_str1 = "Xc: ";
  print_str1 += std::to_string(coords_chunks[0].x);
  print_str1 += "; Yc: ";
  print_str1 += std::to_string(coords_chunks[0].y);
  Scene.DrawStaticString(-0.99, 0.87, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str1);

  // drawing coordinates biomes
  std::string print_str2;
  print_str2 = "Xb: ";
  print_str2 += std::to_string(coords_biomes[0].x);
  print_str2 += "; Yb: ";
  print_str2 += std::to_string(coords_biomes[0].y);
  Scene.DrawStaticString(-0.99, 0.83, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str2);

  Scene.setPerspectiveProjection();

  glutSwapBuffers();
}

// processing keys
void Keyboard(unsigned char key, int x, int y){
  switch (key){
    // escape
    case 27:
      clearTextures();
      glutDestroyWindow(wnd);
    break;
  }
}

// processing keys
void ExtKeyboard(int key, int x, int y){
  switch (key){
    // key left arrow
    case GLUT_KEY_LEFT:
      // if next coordinate x placed in next chunk
      if ((xpos + 1) == (CalculateBorder(xpos, size_zone) + 1)){

        // if next coordinate x of left chunk placed in next biome zone
        if ((coords_chunks[7].x + 1) == (CalculateBorder(coords_chunks[7].x, size_zone_biomes) + 1)){
          // load next horizontal biome zone ([1])
          loaded_biomes[1] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }

        // if next coordinate x of center chunk placed in next biome zone
        if ((coords_chunks[0].x + 1) == (CalculateBorder(coords_chunks[0].x, size_zone_biomes) + 1)){
          // increase coordinates of biome zone
          coords_biomes[0].x++;
          // redefine biome zones
          tmp_biomes[0] = loaded_biomes[0];
          loaded_biomes[0] = loaded_biomes[1];
          loaded_biomes[1] = tmp_biomes[0];
        }

        // changing of position and content of loaded into memory chunks
        MoveChunks(true, true);
      }
      // increase camera and tile coordinates
      xpos_cam += scale;
      xpos++;
    break;

    // key right arrow
    case GLUT_KEY_RIGHT:
      // if next coordinate x placed in next chunk
      if ((xpos - 1) == (CalculateBorder(xpos, size_zone) - size_zone)){

        // if next coordinate x of left chunk placed in next biome zone
        if ((coords_chunks[3].x - 1) == (CalculateBorder(coords_chunks[3].x, size_zone_biomes) - size_zone_biomes)){
          // load next horizontal biome zone ([1])
          loaded_biomes[1] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }

        // if next coordinate x of center chunk placed in next biome zone
        if ((coords_chunks[0].x - 1) == (CalculateBorder(coords_chunks[0].x, size_zone_biomes) - size_zone_biomes)){
          // increase coordinates of biome zone
          coords_biomes[0].x--;
          // redefine biome zones
          tmp_biomes[0] = loaded_biomes[0];
          loaded_biomes[0] = loaded_biomes[1];
          loaded_biomes[1] = tmp_biomes[0];
        }

        // changing of position and content of loaded into memory chunks
        MoveChunks(true, false);
      }
      // increase camera and tile coordinates
      xpos_cam -= scale;
      xpos--;
    break;

    // key up arrow
    case GLUT_KEY_UP:
      // if next coordinate y placed in next chunk
      if ((ypos + 1) == (CalculateBorder(ypos, size_zone) + 1)){

        // if next coordinate y of up chunk placed in next biome zone
        if ((coords_chunks[1].y + 1) == (CalculateBorder(coords_chunks[1].y, size_zone_biomes) + 1)){
          // load next vertical biome zone ([2])
          loaded_biomes[2] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }

        // if next coordinate y of center chunk placed in next biome zone
        if ((coords_chunks[0].y + 1) == (CalculateBorder(coords_chunks[0].y, size_zone_biomes) + 1)){
          // increase coordinates of biome zone
          coords_biomes[0].y++;
          // redefine biome zones
          tmp_biomes[0] = loaded_biomes[1];
          loaded_biomes[1] = loaded_biomes[2];
          loaded_biomes[2] = tmp_biomes[0];
        }

        // changing of position and content of loaded into memory chunks
        MoveChunks(false, true);
      }
      // increase camera and tile coordinates
      ypos_cam += scale;
      ypos++;
    break;

    // key down arrow
    case GLUT_KEY_DOWN:
      // if next coordinate y placed in next chunk
      if ((ypos - 1) == (CalculateBorder(ypos, size_zone) - 64)){

        // if next coordinate y of down chunk placed in next biome zone
        if ((coords_chunks[5].y - 1) == (CalculateBorder(coords_chunks[5].y, size_zone_biomes) - size_zone_biomes)){
          // load next vertical biome zone ([2])
          loaded_biomes[2] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }

        // if next coordinate y of center chunk placed in next biome zone
        if ((coords_chunks[0].y - 1) == (CalculateBorder(coords_chunks[0].y, size_zone_biomes) - size_zone_biomes)){
          // increase coordinates of biome zone
          coords_biomes[0].y--;
          // redefine biome zones
          tmp_biomes[0] = loaded_biomes[1];
          loaded_biomes[1] = loaded_biomes[2];
          loaded_biomes[2] = tmp_biomes[0];
        }

        // changing of position and content of loaded into memory chunks
        MoveChunks(false, false);
      }
      // increase camera and tile coordinates
      ypos_cam -= scale;
      ypos--;
    break;
  }
}

void Idle(){
}

int main(int argc, char *argv[]){
  // getting seed
  // loading world
  std::cout << "Enter seed:";
  std::cin >> str_seed;

  // initializing and create window GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(1920, 1080);
  wnd = glutCreateWindow("Evolution 0.0.8");

  // initializing coordinates of biomes zones
  coords_biomes[0].x = 0;  coords_biomes[0].y = 0;

  // initializing coordinates of chunks
  coords_chunks[0].x = 0;  coords_chunks[0].y = 0;
  coords_chunks[1].x = 0;  coords_chunks[1].y = 1;
  coords_chunks[2].x = -1; coords_chunks[2].y = 1;
  coords_chunks[3].x = -1; coords_chunks[3].y = 0;
  coords_chunks[4].x = -1; coords_chunks[4].y = -1;
  coords_chunks[5].x = 0;  coords_chunks[5].y = -1;
  coords_chunks[6].x = 1;  coords_chunks[6].y = -1;
  coords_chunks[7].x = 1;  coords_chunks[7].y = 0;
  coords_chunks[8].x = 1;  coords_chunks[8].y = 1;

  // loading world
  xpos = 0;
  ypos = 0;
  int_seed = StrToInt(str_seed);
  loaded_biomes[0] = LoadBiomes(int_seed, StrToInt("00"));
  LoadThreeChunks({0, 1, 2},
                  {0, 0},
                  {0, 1},
                  {1, 1});
  LoadThreeChunks({3, 4, 5},
                  {1, 0},
                  {1, -1},
                  {0, -1});
  LoadThreeChunks({6, 7, 8},
                  {-1, -1},
                  {-1, 0},
                  {-1, 1});

  // load Textures
  LoadTextures();

  // defining events of window
  glutDisplayFunc(Draw);
  glutReshapeFunc(Reshape);
  glutIdleFunc(Draw);
  glutKeyboardFunc(Keyboard);
  glutSpecialFunc(ExtKeyboard);

  // fon color
  glClearColor(0.5, 0.5, 0.5, 0.0);
  // smoothing paint of color
  glShadeModel(GL_SMOOTH);
  // modificated perspective
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // processing events of window
  glutMainLoop();
  // clear textures
  clearTextures();
  return 0;
}
