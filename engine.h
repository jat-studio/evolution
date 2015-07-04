#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

/*#######################   classes   #######################*/
class ClassScene{
  private:

  public:
    // enter to ortho mode
    void setOrthographicProjection(){
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
    // exit of ortho mode
    void setPerspectiveProjection(){
      glPopMatrix();

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);

      glColor3f(1.0, 1.0, 1.0);
      glEnable(GL_BLEND);
    }
    // output string
    void DrawStaticString(float x, float y, float z, void *font, std::string input){
      glRasterPos3f(x, y, z);
      unsigned short int len = input.length();
      for(unsigned short int i = 0; i < len; i++){
        glutBitmapCharacter(font, input[i]);
      }
    }
    // loading texture by filename
    void LoadTextureImage(const char *texName, GLuint texture){
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
      // loading texture into memory
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      gluBuild2DMipmaps(GL_TEXTURE_2D, bpp, width, height, type, GL_UNSIGNED_BYTE, data_img);
    }
};

/*#######################   structures   #######################*/
// structure for id chunks
struct tag_id_chunks{
  unsigned short int ida;
  unsigned short int idb;
  unsigned short int idc;
};
// structure for int coordinates of biomes zones
struct tag_coords_biomes{
  int x;
  int y;
};
// structure for int coordinates of chunks
struct tag_coords_chunks{
  int x;
  int y;
};

/*#######################   functions   #######################*/
// calculating left or up border current chunk or biome zone
// and compare it to next coordinate
bool CalculateBorder(int pos, unsigned short int zone, bool increase);

#endif
