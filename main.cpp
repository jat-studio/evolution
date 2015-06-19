#include <GL/glut.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>

#include "worldgen.h"

unsigned short int wnd;            // id GLwindow

const unsigned short int count_tex = 5; // count of textures
GLuint tiles_tex[count_tex - 1];    // index texture of tiles

// parameters of image
int width;
int height;
int bpp;
unsigned int type;
unsigned char* data_img;

// fps drawing
int fps;
long t, dt;
std::string fps_str;

// parameters for load and generate world with seed
tag_biomes load_biomes[1];
tag_tiles load_tiles[num_zones];
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

// loading texture by filename
void LoadTextureImage(const char* texName){
  // initializing il and ilu library
  ilInit();
  iluInit();
  // loading texture
  ilLoad(IL_TYPE_UNKNOWN, texName);
  // processing of errors
  int err = ilGetError();
  if (err != IL_NO_ERROR){
    const char* strError = iluErrorString(err);
    std::cout << "Error loading texture!\n" << texName << "\n" << strError << "\n";
    exit(EXIT_FAILURE);
  }
  // getting parameters of image
  width  = ilGetInteger(IL_IMAGE_WIDTH);
  height = ilGetInteger(IL_IMAGE_HEIGHT);
  bpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
  // getting data of image
  data_img = ilGetData();
  // determination type of image
  switch (bpp){
    case 1:
      type = GL_RGB8;
    break;
    case 3:
      type = GL_RGB;
    break;
    case 4:
      type = GL_RGBA;
    break;
  }
}

// loading textures
void LoadTextures(void){
  // create array of textures
  glGenTextures(count_tex, &tiles_tex[0]);

  // getting data textures of tiles
  LoadTextureImage("grass.png");
  // loading textures into memory
  glBindTexture(GL_TEXTURE_2D, tiles_tex[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);

  // getting data textures of tiles
  LoadTextureImage("stone.png");
  // texture with linear filter
  glBindTexture(GL_TEXTURE_2D, tiles_tex[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);

  // getting data textures of tiles
  LoadTextureImage("water.png");
  // texture with linear filter
  glBindTexture(GL_TEXTURE_2D, tiles_tex[2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);

  // getting data textures of tiles
  LoadTextureImage("water_wave.png");
  // texture with linear filter
  glBindTexture(GL_TEXTURE_2D, tiles_tex[3]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);

  // getting data textures of tiles
  LoadTextureImage("player.tga");
  // texture with linear filter
  glBindTexture(GL_TEXTURE_2D, tiles_tex[4]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);

  // enabling textures
  glEnable(GL_TEXTURE_2D);
}

// deleting textures
void clearTextures(void){
  glDeleteTextures(count_tex, &tiles_tex[0]);
}

// enter to ortho mode
void setOrthographicProjection(void){
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 0, 1920, 1080);
  glMatrixMode(GL_MODELVIEW);
}

// exit of ortho mode
void restorePerspectiveProjection(void){
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

// output string
void DrawStaticString(float x, float y, float z, void *font, std::string input){
  glRasterPos3f(x, y, z);
  unsigned short int len = input.length();
  for(unsigned short int i = 0; i < len; i++){
    glutBitmapCharacter(font, input[i]);
  }
}

//drawing chunk
void DrawChunk(unsigned short int index, int x, int y){
  unsigned short int loop_col, loop_row, tile_get;

  glPushMatrix();
  glTranslatef(x * size_zone, y * size_zone, 0.0);
  for (loop_col = 0; loop_col < size_zone; loop_col++){
    for (loop_row = 0; loop_row < size_zone; loop_row++){
      tile_get = load_tiles[index].tile_id [loop_col] [loop_row];
      /*if (current_biome_id == 1){
        tile_get += 2;
      }*/

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
void DrawPlayer(void){
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

// string >> char_codes >> string
std::string StrToInt(std::string str){
  unsigned short int length = str.length();
  char buffer[3];
  std::string str_out;
  for (unsigned short int i = 0; i < length; i++){
    sprintf(buffer,"%d", (int)str[i]);
    str_out += buffer;
  }
  return str_out;
}

// loading three chunks
void LoadThreeChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks_A, tag_coords_chunks coords_chunks_B, tag_coords_chunks coords_chunks_C){
  load_tiles[id_chunks.ida] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_A.x) + std::to_string(coords_chunks_A.y)), 0);
  load_tiles[id_chunks.idb] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_B.x) + std::to_string(coords_chunks_B.y)), 0);
  load_tiles[id_chunks.idc] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_C.x) + std::to_string(coords_chunks_C.y)), 0);
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
    coords_chunks[0].x = coords_chunks[0].x + coeff;
    coords_chunks[1].x = coords_chunks[1].x + coeff;
    coords_chunks[2].x = coords_chunks[2].x + coeff;
    coords_chunks[3].x = coords_chunks[3].x + coeff;
    coords_chunks[4].x = coords_chunks[4].x + coeff;
    coords_chunks[5].x = coords_chunks[5].x + coeff;
    coords_chunks[6].x = coords_chunks[6].x + coeff;
    coords_chunks[7].x = coords_chunks[7].x + coeff;
    coords_chunks[8].x = coords_chunks[8].x + coeff;
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
    coords_chunks[0].y = coords_chunks[0].y + coeff;
    coords_chunks[1].y = coords_chunks[1].y + coeff;
    coords_chunks[2].y = coords_chunks[2].y + coeff;
    coords_chunks[3].y = coords_chunks[3].y + coeff;
    coords_chunks[4].y = coords_chunks[4].y + coeff;
    coords_chunks[5].y = coords_chunks[5].y + coeff;
    coords_chunks[6].y = coords_chunks[6].y + coeff;
    coords_chunks[7].y = coords_chunks[7].y + coeff;
    coords_chunks[8].y = coords_chunks[8].y + coeff;
  }

  // redefining content of chunks
  load_tiles[id[0]] = load_tiles[id[3]];
  load_tiles[id[1]] = load_tiles[id[4]];
  load_tiles[id[2]] = load_tiles[id[5]];

  load_tiles[id[3]] = load_tiles[id[6]];
  load_tiles[id[4]] = load_tiles[id[7]];
  load_tiles[id[5]] = load_tiles[id[8]];

  // generate three new chunks
  LoadThreeChunks({id[6], id[7], id[8]},
                  {coords_chunks[id[6]].x, coords_chunks[id[6]].y},
                  {coords_chunks[id[7]].x, coords_chunks[id[7]].y},
                  {coords_chunks[id[8]].x, coords_chunks[id[8]].y});
}

// calculating border current chunk or biome zone
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
void Draw(void){
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

  // calculating fps
  fps++;
  t = glutGet(GLUT_ELAPSED_TIME);
  if (t - dt > 1000){
    fps_str = "FPS:" + std::to_string((int)(fps * 1000.0 / (t - dt)));
    dt = t;
    fps = 0;
  }

  // painting text on 2d mode
  glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_BLEND);
  setOrthographicProjection();
  glPushMatrix();
  glLoadIdentity();
  // drawing fps
  DrawStaticString(-0.99, 0.95, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, fps_str);

  // drawing coordinates
  std::string print_str;
  print_str = "X: ";
  print_str += std::to_string(xpos);
  print_str += "; Y: ";
  print_str += std::to_string(ypos);
  DrawStaticString(-0.99, 0.91, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str);

  // drawing coordinates chunks
  std::string print_str1;
  print_str1 = "Xc: ";
  print_str1 += std::to_string(((int)(coords_chunks[0].x / 2)) - 1);
  print_str1 += "; Yc: ";
  print_str1 += std::to_string(((int)(coords_chunks[0].y / 2)) - 1);
  DrawStaticString(-0.99, 0.87, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str1);

  // drawing coordinates biomes
  std::string print_str2;
  print_str2 = "Xb: ";
  print_str2 += std::to_string(coords_biomes[0].x);
  print_str2 += "; Yb: ";
  print_str2 += std::to_string(coords_biomes[0].y);
  DrawStaticString(-0.99, 0.83, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str2);

  glPopMatrix();
  restorePerspectiveProjection();
  glColor3f(1.0, 1.0, 1.0);
  glEnable(GL_BLEND);

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
      xpos_cam += scale;
      xpos++;
      // if current coordinate x is equal to border current chunk
      if (xpos == CalculateBorder(xpos, size_zone)){
        // if current coordinate x of current chunk is equal to border array with id of biomes
        if (coords_chunks[7].x == CalculateBorder(coords_chunks[7].x, size_zone_biomes)){
          // generate new biomes array
          coords_biomes[0].x = coords_biomes[0].x + 1;
          load_biomes[0] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }
        // changing of position and content of loaded into memory chunks
        MoveChunks(true, true);
      }
    break;
    case GLUT_KEY_RIGHT:
      xpos_cam -= scale;
      xpos--;
      if (xpos == CalculateBorder(xpos, size_zone)){
        if (coords_chunks[3].x == CalculateBorder(coords_chunks[3].x, size_zone_biomes)){
          coords_biomes[0].x = coords_biomes[0].x - 1;
          load_biomes[0] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }
        MoveChunks(true, false);
      }
    break;
    case GLUT_KEY_UP:
      ypos_cam += scale;
      ypos++;
      if (ypos == CalculateBorder(ypos, size_zone)){
        if (coords_chunks[1].y == CalculateBorder(coords_chunks[1].y, size_zone_biomes)){
          coords_biomes[0].y = coords_biomes[0].y + 1;
          load_biomes[0] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }
        MoveChunks(false, true);
      }
    break;
    case GLUT_KEY_DOWN:
      ypos_cam -= scale;
      ypos--;
      if (ypos == CalculateBorder(ypos, size_zone)){
        if (coords_chunks[5].y == CalculateBorder(coords_chunks[5].y, size_zone_biomes)){
          coords_biomes[0].y = coords_biomes[0].y - 1;
          load_biomes[0] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
        }
        MoveChunks(false, false);
      }
    break;
  }
}

void Idle(void){
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
  load_biomes[0] = LoadBiomes(int_seed, StrToInt("00"));
//  std::cout << load_biomes[0].biome_id[64, 64];
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
