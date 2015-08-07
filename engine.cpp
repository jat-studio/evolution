using namespace std;
/*Basic c library*/
#include <iostream>
#include <cstring>
#include <array>
#include <vector>
#include <map>

/*Open GL*/
#include "GL/glut.h"
#include "IL/il.h"
#include "IL/ilu.h"

/*My library*/
#include "worldgen.h"
#include "engine.h"

/*#####################Class Scene implementation###################*/
// initialization count of textures
void ClassScene::TexInit(unsigned short int count_tex){
  // count of textures
  ClassScene::count_tex = count_tex;
  // index texture of tiles
  tiles_tex = new GLuint[count_tex];
}

// set 2d mode
void ClassScene::setOrthoProjection(GLsizei Width, GLsizei Height){
  glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_BLEND);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 0, Width, Height);
  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  glLoadIdentity();
}

// set 3d mode
void ClassScene::setPerspectiveProjection(){
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glColor3f(1.0, 1.0, 1.0);
  glEnable(GL_BLEND);
}

//drawing chunk
void ClassScene::DrawChunk(unsigned short int index, int x, int y){
    unsigned short int loop_col, loop_row, tile_get;

    glPushMatrix();
    glTranslatef(x * size_zone, y * size_zone, 0.0);
    for (loop_col = 0; loop_col < size_zone; loop_col++) {
        for (loop_row = 0; loop_row < size_zone; loop_row++) {
            tile_get = ClassScene::loaded_tiles[index].tile_id [loop_col] [loop_row];

            glBindTexture(GL_TEXTURE_2D, ClassScene::tiles_tex[tile_get]);

            glBegin(GL_QUADS);
            glNormal3f(0.0, 0.0, -1.0);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(((size_zone / 2) - loop_row), (((-1 * size_zone) / 2) + loop_col + 1), 0.0);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(((size_zone / 2) - loop_row - 1), (((-1 * size_zone) / 2) + loop_col + 1), 0.0);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(((size_zone / 2) - loop_row - 1), (((-1 * size_zone) / 2) + loop_col + 2), 0.0);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(((size_zone / 2) - loop_row), (((-1 * size_zone) / 2) + loop_col + 2), 0.0);
            glEnd();
        }
    }
    glPopMatrix();
}

// output string
void ClassScene::DrawStaticString(float x, float y, float z, void *font, string input){
  glRasterPos3f(x, y, z);
  unsigned short int len = input.length();
  for(unsigned short int i = 0; i < len; i++){
    glutBitmapCharacter(font, input[i]);
  }
}

// loading texture by filename
void ClassScene::LoadTextureImage(const char *texName, GLuint texture){
  // parameters of image
  int width;
  int height;
  int bpp;
  unsigned int type;
  unsigned char *data_img;

  // loading texture
  ilLoad(IL_TYPE_UNKNOWN, texName);
  // processing of errors
  int err = ilGetError();
  if (err != IL_NO_ERROR){
    const char *strError = iluErrorString(err);
    cout << "Error loading texture!\n" << texName << "\n" << strError << "\n";
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
  // loading texture into memory
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);
}

// loading textures
void ClassScene::LoadTextures(vector<string> texturelist){
    // create array of textures
    glGenTextures(count_tex, &ClassScene::tiles_tex[0]);

    // initializing il and ilu library
    ilInit();
    iluInit();
    // loading textures
    unsigned short int val = 0;
    for(unsigned short int i = 1; i < texturelist.size(); i += 2){
      ClassScene::TextureManager.insert(pair<string, unsigned short int>(texturelist[i], val));
      string str = texturelist[0];
      str += texturelist[i + 1];
      ClassScene::LoadTextureImage(str.c_str(), ClassScene::tiles_tex[val]);
      val++;
    }
    // enabling textures
    glEnable(GL_TEXTURE_2D);
}

// deleting textures
void ClassScene::ClearTextures(){
    glDeleteTextures(ClassScene::count_tex, &ClassScene::tiles_tex[0]);
}

// painting Scene
void ClassScene::Draw(){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(ClassScene::xpos_cam, ClassScene::ypos_cam, -6.0,
            ClassScene::xpos_cam, ClassScene::ypos_cam, 1.0,
            0.0, 1.0, 0.0);

  // painting world
  glScalef(scale, scale, 0.0);

  ClassScene::DrawChunk(0, ClassScene::coords_chunks[0].x, ClassScene::coords_chunks[0].y);
  ClassScene::DrawChunk(1, ClassScene::coords_chunks[1].x, ClassScene::coords_chunks[1].y);
  ClassScene::DrawChunk(2, ClassScene::coords_chunks[2].x, ClassScene::coords_chunks[2].y);
  ClassScene::DrawChunk(3, ClassScene::coords_chunks[3].x, ClassScene::coords_chunks[3].y);
  ClassScene::DrawChunk(4, ClassScene::coords_chunks[4].x, ClassScene::coords_chunks[4].y);
  ClassScene::DrawChunk(5, ClassScene::coords_chunks[5].x, ClassScene::coords_chunks[5].y);
  ClassScene::DrawChunk(6, ClassScene::coords_chunks[6].x, ClassScene::coords_chunks[6].y);
  ClassScene::DrawChunk(7, ClassScene::coords_chunks[7].x, ClassScene::coords_chunks[7].y);
  ClassScene::DrawChunk(8, ClassScene::coords_chunks[8].x, ClassScene::coords_chunks[8].y);

  ClassScene::DrawPlayer();

  glutSwapBuffers();
}

// repainting OpenGL by reshape window
void ClassScene::Reshape(GLsizei Width, GLsizei Height){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glViewport(0, 0, Width, Height);
  gluPerspective(45.0, (GLfloat) Width / (GLfloat) Height, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

//drawing player
void ClassScene::DrawPlayer(){
    glPushMatrix();
    glTranslatef(ClassScene::xpos, ClassScene::ypos, 0.0);
    glBindTexture(GL_TEXTURE_2D, ClassScene::tiles_tex[ClassScene::TextureManager["player"]]);

    glBegin(GL_QUADS);
      glNormal3f(0.0, 0.0, -1.0);
      glTexCoord2f(0.0, 0.0);      glVertex3f(0.0, 1.0, -0.01);
      glTexCoord2f(1.0, 0.0);      glVertex3f(-1.0, 1.0, -0.01);
      glTexCoord2f(1.0, 1.0);      glVertex3f(-1.0, 0.0, -0.01);
      glTexCoord2f(0.0, 1.0);      glVertex3f(0.0, 0.0, -0.01);
    glEnd();

    glPopMatrix();
}

// function for getting id biome from array using on input coordinates of chunk
unsigned short int ClassScene::GetIDBiome(int x, int y, unsigned short int id_chunk){
    // modify coordinates into array id's
    short int coeff_x = ((x + (size_zone_biomes / 2) - 1) -
                                  (size_zone_biomes * ClassScene::coords_biomes[0].x));
    short int coeff_y = ((y + (size_zone_biomes / 2) - 1) -
                                  (size_zone_biomes * ClassScene::coords_biomes[0].y));
    // finding correct array biomes
    unsigned short int id_array_biomes = 0;

    // horizontal border
    if ((coeff_x < 0) or (coeff_x >= size_zone_biomes)){
      id_array_biomes = 1;
      if ((coeff_y < 0) or (coeff_y >= size_zone_biomes)){
        id_array_biomes = 3;
      }
    }

    // vertical border
    if ((coeff_y < 0) or (coeff_y >= size_zone_biomes)){
      id_array_biomes = 1;
      if ((coeff_x < 0) or (coeff_x >= size_zone_biomes)){
        id_array_biomes = 3;
      }
    }

    // return ID Biome
    return ClassScene::loaded_biomes[id_array_biomes].biome_id[coeff_x][coeff_y];
}

// loading three chunks
void ClassScene::LoadThreeChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks_A, tag_coords_chunks coords_chunks_B, tag_coords_chunks coords_chunks_C){
  ClassScene::loaded_tiles[id_chunks.ida] = LoadChunk(
    ClassScene::TextureManager, int_seed, StrToInt(to_string(coords_chunks_A.x) + to_string(coords_chunks_A.y)), ClassScene::GetIDBiome(coords_chunks_A.x, coords_chunks_A.y, id_chunks.ida));
  ClassScene::loaded_tiles[id_chunks.idb] = LoadChunk(
    ClassScene::TextureManager, int_seed, StrToInt(to_string(coords_chunks_B.x) + to_string(coords_chunks_B.y)), ClassScene::GetIDBiome(coords_chunks_B.x, coords_chunks_B.y, id_chunks.idb));
  ClassScene::loaded_tiles[id_chunks.idc] = LoadChunk(
    ClassScene::TextureManager, int_seed, StrToInt(to_string(coords_chunks_C.x) + to_string(coords_chunks_C.y)), ClassScene::GetIDBiome(coords_chunks_C.x, coords_chunks_C.y, id_chunks.idc));
}

// moving and generate chunks
// parameters: horizontal moving or not and increase or decrease coordinates
void ClassScene::MoveChunks(bool horizontal, bool increase){
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
    ClassScene::coords_chunks[0].x += coeff;
    ClassScene::coords_chunks[1].x += coeff;
    ClassScene::coords_chunks[2].x += coeff;
    ClassScene::coords_chunks[3].x += coeff;
    ClassScene::coords_chunks[4].x += coeff;
    ClassScene::coords_chunks[5].x += coeff;
    ClassScene::coords_chunks[6].x += coeff;
    ClassScene::coords_chunks[7].x += coeff;
    ClassScene::coords_chunks[8].x += coeff;
    ClassScene::border_chunk_xpos += (coeff * size_zone);
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
    ClassScene::coords_chunks[0].y += coeff;
    ClassScene::coords_chunks[1].y += coeff;
    ClassScene::coords_chunks[2].y += coeff;
    ClassScene::coords_chunks[3].y += coeff;
    ClassScene::coords_chunks[4].y += coeff;
    ClassScene::coords_chunks[5].y += coeff;
    ClassScene::coords_chunks[6].y += coeff;
    ClassScene::coords_chunks[7].y += coeff;
    ClassScene::coords_chunks[8].y += coeff;
    ClassScene::border_chunk_ypos += (coeff * size_zone);
  }

  // redefining content of chunks
  ClassScene::loaded_tiles[id[0]] = ClassScene::loaded_tiles[id[3]];
  ClassScene::loaded_tiles[id[1]] = ClassScene::loaded_tiles[id[4]];
  ClassScene::loaded_tiles[id[2]] = ClassScene::loaded_tiles[id[5]];

  ClassScene::loaded_tiles[id[3]] = ClassScene::loaded_tiles[id[6]];
  ClassScene::loaded_tiles[id[4]] = ClassScene::loaded_tiles[id[7]];
  ClassScene::loaded_tiles[id[5]] = ClassScene::loaded_tiles[id[8]];

  // generate three new chunks
  ClassScene::LoadThreeChunks({id[6], id[7], id[8]},
    {ClassScene::coords_chunks[id[6]].x, ClassScene::coords_chunks[id[6]].y},
    {ClassScene::coords_chunks[id[7]].x, ClassScene::coords_chunks[id[7]].y},
    {ClassScene::coords_chunks[id[8]].x, ClassScene::coords_chunks[id[8]].y});
}

// moving camera
void ClassScene::MoveCamera(bool horizontal, bool increase){
  if (horizontal){
    if (increase){
      ClassScene::xpos_cam += scale;
      ClassScene::xpos++;
    }
    else{
      ClassScene::xpos_cam -= scale;
      ClassScene::xpos--;
    }
  }
  else{
    if (increase){
      ClassScene::ypos_cam += scale;
      ClassScene::ypos++;
    }
    else{
      ClassScene::ypos_cam -= scale;
      ClassScene::ypos--;
    }
  }
}

// moving biome zones
void ClassScene::MoveBiomes(bool horizontal, bool increase){
  if (horizontal){
    if (increase){
      ClassScene::coords_biomes[0].x++;
      ClassScene::border_biom_xpos += size_zone_biomes;
    }
    else{
      ClassScene::coords_biomes[0].x--;
      ClassScene::border_biom_xpos -= size_zone_biomes;
    }
    ClassScene::tmp_biomes[0] = ClassScene::loaded_biomes[0];
    ClassScene::loaded_biomes[0] = ClassScene::loaded_biomes[1];
    ClassScene::loaded_biomes[1] = ClassScene::tmp_biomes[0];

    ClassScene::tmp_biomes[0] = ClassScene::loaded_biomes[2];
    ClassScene::loaded_biomes[2] = ClassScene::loaded_biomes[3];
    ClassScene::loaded_biomes[3] = ClassScene::tmp_biomes[0];
  }
  else{
    if (increase){
      ClassScene::coords_biomes[0].y++;
      ClassScene::border_biom_ypos += size_zone_biomes;
    }
    else{
      ClassScene::coords_biomes[0].y--;
      ClassScene::border_biom_ypos -= size_zone_biomes;
    }
    ClassScene::tmp_biomes[0] = ClassScene::loaded_biomes[0];
    ClassScene::loaded_biomes[0] = ClassScene::loaded_biomes[2];
    ClassScene::loaded_biomes[2] = ClassScene::tmp_biomes[0];

    ClassScene::tmp_biomes[0] = ClassScene::loaded_biomes[1];
    ClassScene::loaded_biomes[1] = ClassScene::loaded_biomes[3];
    ClassScene::loaded_biomes[3] = ClassScene::tmp_biomes[0];
  }
}

// processing move camera
// variable direction:
// 0 - left
// 1 - right
// 2 - up
// 3 - down
void ClassScene::ProcessMoving(unsigned short int direction){
  // defining variables
  bool Mov_increase;
  bool Mov_horizontal;

  int Mov_pos;
  int Mov_border;

  int Mov_BorderBiom;
  int Mov_BorderChunkCoord;
  int Mov_CenterChunkCoord;
  int Mov_DiagonalChunkCoord1;
  int Mov_DiagonalChunkCoord2;
  int Mov_DiagonalBorderBiom1;
  int Mov_DiagonalBorderBiom2;
  unsigned short int Mov_IDBiomesArray;
  short int Mov_CoeffX;
  short int Mov_CoeffY;
  short int Mov_BiomCoeffX1;
  short int Mov_BiomCoeffY1;
  short int Mov_BiomCoeffX2;
  short int Mov_BiomCoeffY2;

  // initializing variables
  switch (direction){
    // left direction
    case 0:
      Mov_increase = true;
      Mov_horizontal = true;

      Mov_pos = ClassScene::xpos + 1;
      Mov_border = ClassScene::border_chunk_xpos + 1;

      Mov_BorderBiom = ClassScene::border_biom_xpos + 1;
      Mov_BorderChunkCoord = ClassScene::coords_chunks[7].x + 1;
      Mov_CenterChunkCoord = ClassScene::coords_chunks[0].x + 1;
      Mov_DiagonalChunkCoord1 = ClassScene::coords_chunks[1].y + 1;
      Mov_DiagonalChunkCoord2 = ClassScene::coords_chunks[5].y - 1;
      Mov_DiagonalBorderBiom1 = ClassScene::border_biom_ypos + 1;
      Mov_DiagonalBorderBiom2 = ClassScene::border_biom_ypos - size_zone_biomes;
      Mov_IDBiomesArray = 1;
      Mov_CoeffX = 1;
      Mov_CoeffY = 0;
      Mov_BiomCoeffX1 = 1;
      Mov_BiomCoeffY1 = 1;
      Mov_BiomCoeffX2 = 1;
      Mov_BiomCoeffY2 = -1;
    break;
    // right direction
    case 1:
      Mov_increase = false;
      Mov_horizontal = true;

      Mov_pos = ClassScene::xpos - 1;
      Mov_border = ClassScene::border_chunk_xpos - size_zone;

      Mov_BorderBiom = ClassScene::border_biom_xpos - size_zone_biomes;
      Mov_BorderChunkCoord = ClassScene::coords_chunks[3].x - 1;
      Mov_CenterChunkCoord = ClassScene::coords_chunks[0].x - 1;
      Mov_DiagonalChunkCoord1 = ClassScene::coords_chunks[1].y + 1;
      Mov_DiagonalChunkCoord2 = ClassScene::coords_chunks[5].y - 1;
      Mov_DiagonalBorderBiom1 = ClassScene::border_biom_ypos + 1;
      Mov_DiagonalBorderBiom2 = ClassScene::border_biom_ypos - size_zone_biomes;
      Mov_IDBiomesArray = 1;
      Mov_CoeffX = -1;
      Mov_CoeffY = 0;
      Mov_BiomCoeffX1 = -1;
      Mov_BiomCoeffY1 = 1;
      Mov_BiomCoeffX2 = -1;
      Mov_BiomCoeffY2 = -1;
    break;
    // up direction
    case 2:
      Mov_increase = true;
      Mov_horizontal = false;

      Mov_pos = ClassScene::ypos + 1;
      Mov_border = ClassScene::border_chunk_ypos + 1;

      Mov_BorderBiom = ClassScene::border_biom_ypos + 1;
      Mov_BorderChunkCoord = ClassScene::coords_chunks[1].y + 1;
      Mov_CenterChunkCoord = ClassScene::coords_chunks[0].y + 1;
      Mov_DiagonalChunkCoord1 = ClassScene::coords_chunks[7].x + 1;
      Mov_DiagonalChunkCoord2 = ClassScene::coords_chunks[3].x - 1;
      Mov_DiagonalBorderBiom1 = ClassScene::border_biom_xpos + 1;
      Mov_DiagonalBorderBiom2 = ClassScene::border_biom_xpos - size_zone_biomes;
      Mov_IDBiomesArray = 2;
      Mov_CoeffX = 0;
      Mov_CoeffY = 1;
      Mov_BiomCoeffX1 = 1;
      Mov_BiomCoeffY1 = 1;
      Mov_BiomCoeffX2 = -1;
      Mov_BiomCoeffY2 = 1;
    break;
    // down direction
    case 3:
      Mov_increase = false;
      Mov_horizontal = false;

      Mov_pos = ClassScene::ypos - 1;
      Mov_border = ClassScene::border_chunk_ypos - size_zone;

      Mov_BorderBiom = ClassScene::border_biom_ypos - size_zone_biomes;
      Mov_BorderChunkCoord = ClassScene::coords_chunks[5].y - 1;
      Mov_CenterChunkCoord = ClassScene::coords_chunks[0].y - 1;
      Mov_DiagonalChunkCoord1 = ClassScene::coords_chunks[7].x + 1;
      Mov_DiagonalChunkCoord2 = ClassScene::coords_chunks[3].x - 1;
      Mov_DiagonalBorderBiom1 = ClassScene::border_biom_xpos + 1;
      Mov_DiagonalBorderBiom2 = ClassScene::border_biom_xpos - size_zone_biomes;
      Mov_IDBiomesArray = 2;
      Mov_CoeffX = 0;
      Mov_CoeffY = -1;
      Mov_BiomCoeffX1 = 1;
      Mov_BiomCoeffY1 = -1;
      Mov_BiomCoeffX2 = -1;
      Mov_BiomCoeffY2 = -1;
    break;
  }

  // if next coordinate placed in next chunk
  if (Mov_pos == Mov_border){
    // if next coordinate of border chunk placed in next biome zone
    if (Mov_BorderChunkCoord == Mov_BorderBiom){
      // load next biome zone
      loaded_biomes[Mov_IDBiomesArray] =
      LoadBiomes(int_seed, StrToInt(to_string(ClassScene::coords_biomes[0].x + Mov_CoeffX)
                 + to_string(ClassScene::coords_biomes[0].y + Mov_CoeffY)));
      // load next diagonal biome zone ([3])
      if (Mov_DiagonalChunkCoord1 == Mov_DiagonalBorderBiom1){
        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(to_string(ClassScene::coords_biomes[0].x + Mov_BiomCoeffX1) + to_string(ClassScene::coords_biomes[0].y + Mov_BiomCoeffY1)));
      }
      if (Mov_DiagonalChunkCoord2 == Mov_DiagonalBorderBiom2){
        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(to_string(ClassScene::coords_biomes[0].x + Mov_BiomCoeffX2) + to_string(ClassScene::coords_biomes[0].y + Mov_BiomCoeffY2)));
      }
    }

    // if next coordinate x of center chunk placed in next biome zone
    if (Mov_CenterChunkCoord == Mov_BorderBiom){
      // changing coordinates of biome zone
      ClassScene::MoveBiomes(Mov_horizontal, Mov_increase);
    }
    // changing of position and content of loaded into memory chunks
    ClassScene::MoveChunks(Mov_horizontal, Mov_increase);
  }
  // changing camera and tile coordinates
  ClassScene::MoveCamera(Mov_horizontal, Mov_increase);
}

// destructor
ClassScene::~ClassScene(){
  ClassScene::TextureManager.clear();
}

/*#####################Class Console implementation###################*/
// painting Console
void ClassConsole::Draw(ClassScene &Object, unsigned short int console, unsigned short int wnd){

  glutSetWindow(console);
  if (!ClassConsole::visible){
    glutHideWindow();
  }
  else{
    glutShowWindow();
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0, 0.0, 0.0);

  // drawing fps
  ClassConsole::fps++;
  ClassConsole::t = glutGet(GLUT_ELAPSED_TIME);
  if (ClassConsole::t - ClassConsole::dt > 1000){
    ClassConsole::fps_str = "FPS:" + std::to_string((int)(ClassConsole::fps * 1000.0 / (ClassConsole::t - ClassConsole::dt)));
    ClassConsole::dt = ClassConsole::t;
    ClassConsole::fps = 0;
  }
  Object.DrawStaticString(-0.99, 0.90, 0.0, GLUT_BITMAP_8_BY_13, ClassConsole::fps_str);

  // drawing coordinates
  std::string print_str;
  print_str = "X: ";
  print_str += std::to_string(Object.xpos);
  print_str += "; Y: ";
  print_str += std::to_string(Object.ypos);
  Object.DrawStaticString(-0.99, 0.75, 0.0, GLUT_BITMAP_8_BY_13, print_str);

  // drawing coordinates chunks
  std::string print_str1;
  print_str1 = "Xc: ";
  print_str1 += std::to_string(Object.coords_chunks[0].x);
  print_str1 += "; Yc: ";
  print_str1 += std::to_string(Object.coords_chunks[0].y);
  Object.DrawStaticString(-0.99, 0.60, 0.0, GLUT_BITMAP_8_BY_13, print_str1);

  // drawing coordinates biomes
  std::string print_str2;
  print_str2 = "Xb: ";
  print_str2 += std::to_string(Object.coords_biomes[0].x);
  print_str2 += "; Yb: ";
  print_str2 += std::to_string(Object.coords_biomes[0].y);
  Object.DrawStaticString(-0.99, 0.45, 0.0, GLUT_BITMAP_8_BY_13, print_str2);

  Object.DrawStaticString(-0.99, 0.30, 0.0, GLUT_BITMAP_8_BY_13, ClassConsole::current_key);

  glFlush();

  glutSetWindow(wnd);
}

// set 2d mode
void ClassConsole::Reshape(GLsizei Width, GLsizei Height){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, Width, Height);
  gluOrtho2D(0, 0, Width, Height);
  glMatrixMode(GL_MODELVIEW);
}
