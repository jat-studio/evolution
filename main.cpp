#include <GL/glut.h>
#include <IL/il.h>
#include <IL/ilu.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "worldgen.h"

unsigned short int wnd;            // id GLwindow

const unsigned short int count_tex = 4; // count of textures
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
tag_tiles load_tiles[num_zones];
std::string str_seed, int_seed;

// parameters for moving camera
float xpos = 0.0;
float ypos = 0.0;

// structure for id chunks
struct tag_id_chunks{
  unsigned short int idx;
  unsigned short int idy;
  unsigned short int idz;
};
tag_id_chunks id_chunks;

// structure for coordinates of chunks
struct tag_coords_chunks{
  int x0;
  int y0;
  int x1;
  int y1;
  int x2;
  int y2;
};
tag_coords_chunks coords_chunks;

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
  LoadTextureImage("player.tga");
  // texture with linear filter
  glBindTexture(GL_TEXTURE_2D, tiles_tex[3]);
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
  glutBitmapCharacter(font, input[0]);
  glutBitmapCharacter(font, input[1]);
  glutBitmapCharacter(font, input[2]);
  glutBitmapCharacter(font, input[3]);
  glutBitmapCharacter(font, input[4]);
  glutBitmapCharacter(font, input[5]);
  glutBitmapCharacter(font, input[6]);
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

//drawing chunk
void DrawPlayer(void){
  glPushMatrix();
  glTranslatef((xpos / scale), (ypos / scale), 0.0);
  glBindTexture(GL_TEXTURE_2D, tiles_tex[3]);
  glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 1.0, -0.01);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, -0.01);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 0.0, -0.01);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, -0.01);
  glEnd();
  glPopMatrix();
}

// painting Scene
void Draw(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(xpos, ypos, -6.0,
            xpos, ypos, 1.0,
            0.0, 1.0, 0.0);

  // painting world
  glScalef(scale, scale, 0.0);

  DrawChunk(0, 0.0, 0.0);
  DrawChunk(1, 0.0, 1.0);
  DrawChunk(2, -1.0, 1.0);
  DrawChunk(3, -1.0, 0.0);
  DrawChunk(4, -1.0, -1.0);
  DrawChunk(5, 0.0, -1.0);
  DrawChunk(6, 1.0, -1.0);
  DrawChunk(7, 1.0, 0.0);
  DrawChunk(8, 1.0, 1.0);

  DrawPlayer();

  // calculating fps
  fps++;
  t = glutGet(GLUT_ELAPSED_TIME);
  if (t - dt > 1000){
    fps_str = "FPS:" + std::to_string(fps * 1000.0 / (t - dt));
    dt = t;
    fps = 0;
  }

  // painting text on 2d mode
  glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_BLEND);
  setOrthographicProjection();
  glPushMatrix();
  glLoadIdentity();
  DrawStaticString(-0.99, 0.95, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, fps_str);

  DrawStaticString(-0.99, 0.91, 0.0, GLUT_BITMAP_TIMES_ROMAN_24,
                   std::to_string(((int)((xpos / scale) - 0.1)) / 64));

  DrawStaticString(-0.99, 0.87, 0.0, GLUT_BITMAP_TIMES_ROMAN_24,
                   std::to_string((int)((xpos / scale) - 0.1)));

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
void LoadChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks){
  load_tiles[id_chunks.idx] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks.x0)) + StrToInt(std::to_string(coords_chunks.y0)));
  load_tiles[id_chunks.idy] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks.x1)) + StrToInt(std::to_string(coords_chunks.y1)));
  load_tiles[id_chunks.idz] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks.x2)) + StrToInt(std::to_string(coords_chunks.y2)));
}

// processing keys
void ExtKeyboard(int key, int x, int y){
  switch (key){
    case GLUT_KEY_LEFT:
      xpos += scale;
      if (((int)(xpos / scale) % size_zone) == 32){
        std::cout << "load!\n";
        /*float coeffx = 0;
        float coeffy = 0;
        if (xpos > 0)
          coeffx = 0.1;
        else
          coeffx = -0.1;
        if (ypos > 0)
          coeffy = 0.1;
        else
          coeffy = -0.1;
        int x0 = ((int)((xpos / scale) + coeffx)) / size_zone;
        int y0 = ((int)((ypos / scale) + coeffy)) / size_zone;
        LoadChunks({6, 7, 8},
                   {x0 + 1, y0 - 1,
                    x0 + 1, y0,
                    x0 + 1, y0 + 1});*/
      }
    break;
    case GLUT_KEY_RIGHT:
      xpos -= scale;
    break;
    case GLUT_KEY_UP:
      ypos += scale;
    break;
    case GLUT_KEY_DOWN:
      ypos -= scale;
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
  wnd = glutCreateWindow("Evolution 0.0.0");

  // loading world
  int_seed = StrToInt(str_seed);
  LoadChunks({0, 1, 2},
             {0, 0,
              0, 1,
              1, 1});
  LoadChunks({3, 4, 5},
             {1, 0,
              1, -1,
              0, -1});
  LoadChunks({6, 7, 8},
             {-1, -1,
              -1, 0,
              -1, 1});

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
