using namespace std;
/*Basic c library*/
#include <iostream>
#include <vector>

/*Open GL*/
#include "GL/glut.h"
#include "IL/il.h"
#include "IL/ilu.h"

/*My library*/
#include "worldgen.h"
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

/*#####################Class Scene implementation###################*/
// set 2d mode
void ClassScene::setOrthographicProjection(){
  glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_BLEND);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 0, 1920, 1080);
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
    glGenTextures(ClassScene::count_tex, &ClassScene::tiles_tex[0]);

    // initializing il and ilu library
    ilInit();
    iluInit();
    // loading textures
    for(uint i=0;i<texturelist.size();i++){
      ClassScene::LoadTextureImage(texturelist[i].c_str(), ClassScene::tiles_tex[i]);
    }
    // enabling textures
    glEnable(GL_TEXTURE_2D);
}

// deleting textures
void ClassScene::ClearTextures(){
    glDeleteTextures(ClassScene::count_tex, &ClassScene::tiles_tex[0]);
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

/*#####################Class Console implementation###################*/
// painting Console
void ClassConsole::Draw(unsigned short int console, unsigned short int wnd){

  glutSetWindow(console);
  //glutHideWindow();

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
  ClassScene::DrawStaticString(-0.99, 0.90, 0.0, GLUT_BITMAP_8_BY_13, ClassConsole::fps_str);

  // drawing coordinates
  std::string print_str;
  print_str = "X: ";
  print_str += std::to_string(ClassScene::xpos);
  print_str += "; Y: ";
  print_str += std::to_string(ClassScene::ypos);
  ClassScene::DrawStaticString(-0.99, 0.75, 0.0, GLUT_BITMAP_8_BY_13, print_str);

  // drawing coordinates chunks
  std::string print_str1;
  print_str1 = "Xc: ";
  print_str1 += std::to_string(ClassScene::coords_chunks[0].x);
  print_str1 += "; Yc: ";
  print_str1 += std::to_string(ClassScene::coords_chunks[0].y);
  ClassScene::DrawStaticString(-0.99, 0.60, 0.0, GLUT_BITMAP_8_BY_13, print_str1);

  // drawing coordinates biomes
  std::string print_str2;
  print_str2 = "Xb: ";
  print_str2 += std::to_string(ClassScene::coords_biomes[0].x);
  print_str2 += "; Yb: ";
  print_str2 += std::to_string(ClassScene::coords_biomes[0].y);
  ClassScene::DrawStaticString(-0.99, 0.45, 0.0, GLUT_BITMAP_8_BY_13, print_str2);

  ClassScene::DrawStaticString(-0.99, 0.30, 0.0, GLUT_BITMAP_8_BY_13, ClassConsole::current_key);

  glFlush();

  glutSetWindow(wnd);
}
