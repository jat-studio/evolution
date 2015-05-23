#include <GL/glut.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "worldgen.h"

unsigned short int wnd;            // id GLwindow

GLuint tiles_tex[2];    // index texture of tiles

// parameters of image
int width;
int height;
int bpp;
unsigned int type;
unsigned char* data_img;

bool    light = false;      // lights on or off
bool    blend = false;      // blending on or off
GLfloat LightAmbient[] = {0.5, 0.5, 0.5, 1.0}; // array of ambient light
GLfloat LightDiffuse[] = {1.0, 1.0, 1.0, 1.0}; // array of diffuse light
GLfloat LightPosition[] = {0.5, 0.5, 0.5, 1.0}; // coordinates of light source

// fps drawing
int fps;
long t, dt;
char fps_str[50];

tag_tiles load_tiles[num_zones];
std::string str_seed, int_seed;

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
  glGenTextures(3, &tiles_tex[0]);

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

  // enabling textures
  glEnable(GL_TEXTURE_2D);
}

// deleting textures
void clearTextures(void){
  glDeleteTextures(3, &tiles_tex[0]);
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
void DrawStaticString(float x, float y, float z, void *font, char *string){
  char *c;
  glRasterPos3f(x, y, z);
  for (c = string; *c != '\0'; c++){
    glutBitmapCharacter(font, *c);
  }
}

// repainting OpenGL by reshape window
void Reshape(GLsizei Width, GLsizei Height){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, Width, Height);
  gluPerspective(45.0, (GLfloat)Width/(GLfloat)Height, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

//drawing chunk
void DrawChunk(unsigned short int index, float x, float y){
  unsigned short int loop_col, loop_row, tile_get;

  glPushMatrix();
  glTranslatef(x * size_zone, y * size_zone, 0.0);
  for (loop_col = 0; loop_col < size_zone; loop_col++){
    for (loop_row = 0; loop_row < size_zone; loop_row++){
      tile_get = load_tiles[index].tile_id [loop_col] [loop_row];

      glBindTexture(GL_TEXTURE_2D, tiles_tex[tile_get]);

      glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, -1.0);
        glTexCoord2f(0.0, 0.0); glVertex3f(((size_zone / 2) - loop_row), (((-1 * size_zone) / 2) + loop_col + 1), 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(((size_zone / 2) - loop_row - 1), (((-1 * size_zone) / 2) + loop_col + 1), 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(((size_zone / 2) - loop_row - 1), (((-1 * size_zone) / 2) + loop_col), 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(((size_zone / 2) - loop_row), (((-1 * size_zone) / 2) + loop_col), 0.0);
      glEnd();
    }
  }
  glPopMatrix();
}

// painting Scene
void Draw(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, -6.0,
            0.0, 0.0, 1.0,
            0.0, 1.0, 0.0);

  // painting world
  glScalef(0.07, 0.07, 0.0);

  DrawChunk(0, 0.0, 0.0);
  DrawChunk(1, 0.0, 1.0);
  DrawChunk(2, -1.0, 1.0);
  DrawChunk(3, -1.0, 0.0);
  DrawChunk(4, -1.0, -1.0);
  DrawChunk(5, 0.0, -1.0);
  DrawChunk(6, 1.0, -1.0);
  DrawChunk(7, 1.0, 0.0);
  DrawChunk(8, 1.0, 1.0);

  // calculating fps
  fps++;
  t = glutGet(GLUT_ELAPSED_TIME);
  if (t - dt > 1000){
    sprintf(fps_str, "FPS:%4.2f", fps * 1000.0 / (t - dt));
    dt = t;
    fps = 0;
  }

  // painting text on 2d mode
  glColor3f(0.0, 0.0, 0.0);
  setOrthographicProjection();
  glPushMatrix();
  glLoadIdentity();
  DrawStaticString(-0.99, 0.95, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, fps_str);
  glPopMatrix();
  restorePerspectiveProjection();
  glColor3f(1.0, 1.0, 1.0);

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

void Idle(void){
}

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

int main(int argc, char *argv[]){
  // getting seed
  // loading world
  std::cout << "Enter seed:";
  std::cin >> str_seed;

  // initializing and create window GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(1920, 1080);
  wnd = glutCreateWindow("Evolution 0.0.0");

  // loading world
  int_seed = StrToInt(str_seed);
  load_tiles[0] = LoadChunk(int_seed, "4848");
  load_tiles[1] = LoadChunk(int_seed, "4849");
  load_tiles[2] = LoadChunk(int_seed, "4949");
  load_tiles[3] = LoadChunk(int_seed, "4948");
  load_tiles[4] = LoadChunk(int_seed, "494549");
  load_tiles[5] = LoadChunk(int_seed, "484549");
  load_tiles[6] = LoadChunk(int_seed, "45494549");
  load_tiles[7] = LoadChunk(int_seed, "454948");
  load_tiles[8] = LoadChunk(int_seed, "454949");

  // load Textures
  LoadTextures();

  // defining events of window
  glutDisplayFunc(Draw);
  glutReshapeFunc(Reshape);
  glutIdleFunc(Draw);
  glutKeyboardFunc(Keyboard);

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
