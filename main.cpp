#include <GL/glut.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cstring>

int wnd;            // id GLwindow

GLuint Font[1];     // index texture of font
GLuint Tiles[1];    // index texture of tiles

// parameters of image
int width = 0;
int height = 0;
int bpp = 0;
unsigned int type;
unsigned char* data_img;

bool    light = false;      // lights on or off
bool    blend = false;      // blending on or off
GLfloat LightAmbient[] = {0.5, 0.5, 0.5, 1.0}; // array of ambient light
GLfloat LightDiffuse[] = {1.0, 1.0, 1.0, 1.0}; // array of diffuse light
GLfloat LightPosition[] = {0.5, 0.5, 0.5, 1.0}; // coordinates of light source

// output string
void DrawString(float x, float y, float z, void *font, char *string){
  char *c;
  glRasterPos3f(x, y,z);
  for (c = string; *c != '\0'; c++){
    glutBitmapCharacter(font, *c);
  }
}
// loading texture by filename
void LoadTexture(const char* texName){
  // initializing il and ilu library
  ilInit();
  iluInit();
  // loading texture
  char ext[5] = "";
  ext[0] = texName[strlen(texName) - 4];
  ext[1] = texName[strlen(texName) - 3];
  ext[2] = texName[strlen(texName) - 2];
  ext[3] = texName[strlen(texName) - 1];
  ext[4] = NULL;
  if (strcmp(ext, ".bmp") == 0){
    ilLoad(IL_BMP, texName);
  }
  if (strcmp(ext, ".tga") == 0){
    ilLoad(IL_TGA, texName);
  }
  // processing of errors
  int err = ilGetError();
  if (err != IL_NO_ERROR){
    const char* strError = iluErrorString(err);
    printf("Error loading texture!\n");
    printf(strError);
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

// repainting OpenGL by reshape window
void Reshape(GLsizei Width, GLsizei Height){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, Width, Height);
  gluPerspective(45.0, (GLfloat)Width/(GLfloat)Height, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

// painting Scene
void Draw(void){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glPushMatrix();
  glTranslatef(0.0, 0.0, -6);
  glBindTexture(GL_TEXTURE_2D, Tiles[0]);
  glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.125, -0.125, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.125, -0.125, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.125, 0.125, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.125, 0.125, 0.0);
  glEnd();
  DrawString(0.0, 0.5, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, "Hello!");
  glPopMatrix();

  glutSwapBuffers();
}

// processing keys
void Keyboard(unsigned char key, int x, int y){
  switch (key){
    // escape
    case 27:
      glDeleteTextures(1, &Tiles[0]);
      glutDestroyWindow(wnd);
    case 'l':
      if (light){
        glDisable(GL_LIGHTING);
        light = false;
      }
      else{
        glEnable(GL_LIGHTING);
        light = true;
      }
    break;
    case 'b':
      if (blend){
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        blend = false;
      }
      else{
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        blend = true;
      }
    break;
  }
}

int main(int argc, char *argv[]){
  // initializing and create window GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(1920, 1080);
  wnd = glutCreateWindow("Evolution 0.0.0");

  // getting data of textures of cube
  LoadTexture("grass.bmp");
  // loading textures into memory
  glGenTextures(1, &Tiles[0]);
  // texture with linear filter
  glBindTexture(GL_TEXTURE_2D, Tiles[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);
  // enabling textures
  glEnable(GL_TEXTURE_2D);

  // installing fon light
  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  // installing diffuse light
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  // position of light
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
  // enable permission on light
  glEnable(GL_LIGHT1);

  // defining events of window
  glutDisplayFunc(Draw);
  glutReshapeFunc(Reshape);
  glutIdleFunc(Draw);
  glutKeyboardFunc(Keyboard);

  // init GL
  // fon color
  glClearColor(0.5, 0.5, 0.5, 0);
  // depth test
  glClearDepth(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  // smoothing paint of color
  glShadeModel(GL_SMOOTH);
  // blending
  glColor4f(1.0, 1.0, 1.0, 0.5); // full brightness 50% blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  // modificated perspective
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // processing events of window
  glutMainLoop();
  // clear textures
  glDeleteTextures(1, &Tiles[0]);
  return 0;
}
