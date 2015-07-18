using namespace std;
/*Basic c library*/
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>
#include <vector>

/*Open GL*/
#include "GL/glut.h"
#include "IL/il.h"
#include "IL/ilu.h"

/*My library*/
#include "worldgen.h"
#include "engine.h"

/*Unix environment*/
#include <unistd.h>
#include <linux/limits.h>
#include <typeinfo>

/*Config ini function*/
#include "config.h"
#include "minini/minIni.h"

unsigned short int wnd, console; // id GLwindow

// parameters for load and generate world with seed
tag_biomes loaded_biomes[4]; // three biome zones (current vertical, horizontal and diagonal next zone)
tag_biomes tmp_biomes[1]; // temporary structure for redefine loaded_biomes
string str_seed, int_seed;

// Class for basic functions
ClassScene Scene;
// Class for console
ClassConsole Console;

//return path to executing programm
char* getPath() {
    char path[PATH_MAX];
    int cnt;
    if ((cnt = readlink("/proc/self/exe", path, PATH_MAX)) > 0) {
        path[cnt] = 0;
    }
    char* getpath = (char *) malloc(sizeof (char) * (cnt + 1));
    strncpy(getpath, path, cnt);
    getpath[cnt + 1] = 0;
    return getpath;
}

//drawing player
void DrawPlayer() {
    glPushMatrix();
    glTranslatef(Scene.xpos, Scene.ypos, 0.0);
    glBindTexture(GL_TEXTURE_2D, Scene.tiles_tex[4]);

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
unsigned short int GetIDBiome(int x, int y, unsigned short int id_chunk) {
    // modify coordinates into array id's
    short int coeff_x = ((x + (size_zone_biomes / 2) - 1) -
                                  (size_zone_biomes * Scene.coords_biomes[0].x));
    short int coeff_y = ((y + (size_zone_biomes / 2) - 1) -
                                  (size_zone_biomes * Scene.coords_biomes[0].y));
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
    return loaded_biomes[id_array_biomes].biome_id[coeff_x][coeff_y];
}

// loading three chunks
void LoadThreeChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks_A, tag_coords_chunks coords_chunks_B, tag_coords_chunks coords_chunks_C) {
    Scene.loaded_tiles[id_chunks.ida] = LoadChunk(int_seed, StrToInt(to_string(coords_chunks_A.x) + to_string(coords_chunks_A.y)), GetIDBiome(coords_chunks_A.x, coords_chunks_A.y, id_chunks.ida));
    Scene.loaded_tiles[id_chunks.idb] = LoadChunk(int_seed, StrToInt(to_string(coords_chunks_B.x) + to_string(coords_chunks_B.y)), GetIDBiome(coords_chunks_B.x, coords_chunks_B.y, id_chunks.idb));
    Scene.loaded_tiles[id_chunks.idc] = LoadChunk(int_seed, StrToInt(to_string(coords_chunks_C.x) + to_string(coords_chunks_C.y)), GetIDBiome(coords_chunks_C.x, coords_chunks_C.y, id_chunks.idc));
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
    Scene.coords_chunks[0].x += coeff;
    Scene.coords_chunks[1].x += coeff;
    Scene.coords_chunks[2].x += coeff;
    Scene.coords_chunks[3].x += coeff;
    Scene.coords_chunks[4].x += coeff;
    Scene.coords_chunks[5].x += coeff;
    Scene.coords_chunks[6].x += coeff;
    Scene.coords_chunks[7].x += coeff;
    Scene.coords_chunks[8].x += coeff;
    Scene.border_chunk_xpos += (coeff * size_zone);
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
    Scene.coords_chunks[0].y += coeff;
    Scene.coords_chunks[1].y += coeff;
    Scene.coords_chunks[2].y += coeff;
    Scene.coords_chunks[3].y += coeff;
    Scene.coords_chunks[4].y += coeff;
    Scene.coords_chunks[5].y += coeff;
    Scene.coords_chunks[6].y += coeff;
    Scene.coords_chunks[7].y += coeff;
    Scene.coords_chunks[8].y += coeff;
    Scene.border_chunk_ypos += (coeff * size_zone);
  }

  // redefining content of chunks
  Scene.loaded_tiles[id[0]] = Scene.loaded_tiles[id[3]];
  Scene.loaded_tiles[id[1]] = Scene.loaded_tiles[id[4]];
  Scene.loaded_tiles[id[2]] = Scene.loaded_tiles[id[5]];

  Scene.loaded_tiles[id[3]] = Scene.loaded_tiles[id[6]];
  Scene.loaded_tiles[id[4]] = Scene.loaded_tiles[id[7]];
  Scene.loaded_tiles[id[5]] = Scene.loaded_tiles[id[8]];

  // generate three new chunks
  LoadThreeChunks({id[6], id[7], id[8]},
    {
        Scene.coords_chunks[id[6]].x, Scene.coords_chunks[id[6]].y
    },
    {
        Scene.coords_chunks[id[7]].x, Scene.coords_chunks[id[7]].y
    },
    {
        Scene.coords_chunks[id[8]].x, Scene.coords_chunks[id[8]].y
    });
}

// moving biome zones
void MoveBiomes(bool horizontal, bool increase){
  if (horizontal){
    if (increase){
      Scene.coords_biomes[0].x++;
      Scene.border_biom_xpos += size_zone_biomes;
    }
    else{
      Scene.coords_biomes[0].x--;
      Scene.border_biom_xpos -= size_zone_biomes;
    }
    tmp_biomes[0] = loaded_biomes[0];
    loaded_biomes[0] = loaded_biomes[1];
    loaded_biomes[1] = tmp_biomes[0];

    tmp_biomes[0] = loaded_biomes[2];
    loaded_biomes[2] = loaded_biomes[3];
    loaded_biomes[3] = tmp_biomes[0];
  }
  else{
    if (increase){
      Scene.coords_biomes[0].y++;
      Scene.border_biom_ypos += size_zone_biomes;
    }
    else{
      Scene.coords_biomes[0].y--;
      Scene.border_biom_ypos -= size_zone_biomes;
    }
    tmp_biomes[0] = loaded_biomes[0];
    loaded_biomes[0] = loaded_biomes[2];
    loaded_biomes[2] = tmp_biomes[0];

    tmp_biomes[0] = loaded_biomes[1];
    loaded_biomes[1] = loaded_biomes[3];
    loaded_biomes[3] = tmp_biomes[0];
  }
}

// moving camera
void MoveCamera(bool horizontal, bool increase){
  if (horizontal){
    if (increase){
      Scene.xpos_cam += scale;
      Scene.xpos++;
    }
    else{
      Scene.xpos_cam -= scale;
      Scene.xpos--;
    }
  }
  else{
    if (increase){
      Scene.ypos_cam += scale;
      Scene.ypos++;
    }
    else{
      Scene.ypos_cam -= scale;
      Scene.ypos--;
    }
  }
}

// processing move camera
// variable direction:
// 0 - left
// 1 - right
// 2 - up
// 3 - down
void ProcessMoving(unsigned short int direction){
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

      Mov_pos = Scene.xpos + 1;
      Mov_border = Scene.border_chunk_xpos + 1;

      Mov_BorderBiom = Scene.border_biom_xpos + 1;
      Mov_BorderChunkCoord = Scene.coords_chunks[7].x + 1;
      Mov_CenterChunkCoord = Scene.coords_chunks[0].x + 1;
      Mov_DiagonalChunkCoord1 = Scene.coords_chunks[1].y + 1;
      Mov_DiagonalChunkCoord2 = Scene.coords_chunks[5].y - 1;
      Mov_DiagonalBorderBiom1 = Scene.border_biom_ypos + 1;
      Mov_DiagonalBorderBiom2 = Scene.border_biom_ypos - size_zone_biomes;
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

      Mov_pos = Scene.xpos - 1;
      Mov_border = Scene.border_chunk_xpos - size_zone;

      Mov_BorderBiom = Scene.border_biom_xpos - size_zone_biomes;
      Mov_BorderChunkCoord = Scene.coords_chunks[3].x - 1;
      Mov_CenterChunkCoord = Scene.coords_chunks[0].x - 1;
      Mov_DiagonalChunkCoord1 = Scene.coords_chunks[1].y + 1;
      Mov_DiagonalChunkCoord2 = Scene.coords_chunks[5].y - 1;
      Mov_DiagonalBorderBiom1 = Scene.border_biom_ypos + 1;
      Mov_DiagonalBorderBiom2 = Scene.border_biom_ypos - size_zone_biomes;
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

      Mov_pos = Scene.ypos + 1;
      Mov_border = Scene.border_chunk_ypos + 1;

      Mov_BorderBiom = Scene.border_biom_ypos + 1;
      Mov_BorderChunkCoord = Scene.coords_chunks[1].y + 1;
      Mov_CenterChunkCoord = Scene.coords_chunks[0].y + 1;
      Mov_DiagonalChunkCoord1 = Scene.coords_chunks[7].x + 1;
      Mov_DiagonalChunkCoord2 = Scene.coords_chunks[3].x - 1;
      Mov_DiagonalBorderBiom1 = Scene.border_biom_xpos + 1;
      Mov_DiagonalBorderBiom2 = Scene.border_biom_xpos - size_zone_biomes;
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

      Mov_pos = Scene.ypos - 1;
      Mov_border = Scene.border_chunk_ypos - size_zone;

      Mov_BorderBiom = Scene.border_biom_ypos - size_zone_biomes;
      Mov_BorderChunkCoord = Scene.coords_chunks[5].y - 1;
      Mov_CenterChunkCoord = Scene.coords_chunks[0].y - 1;
      Mov_DiagonalChunkCoord1 = Scene.coords_chunks[7].x + 1;
      Mov_DiagonalChunkCoord2 = Scene.coords_chunks[3].x - 1;
      Mov_DiagonalBorderBiom1 = Scene.border_biom_xpos + 1;
      Mov_DiagonalBorderBiom2 = Scene.border_biom_xpos - size_zone_biomes;
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
      LoadBiomes(int_seed, StrToInt(to_string(Scene.coords_biomes[0].x + Mov_CoeffX)
                 + to_string(Scene.coords_biomes[0].y + Mov_CoeffY)));
      // load next diagonal biome zone ([3])
      if (Mov_DiagonalChunkCoord1 == Mov_DiagonalBorderBiom1){
        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(to_string(Scene.coords_biomes[0].x + Mov_BiomCoeffX1) + to_string(Scene.coords_biomes[0].y + Mov_BiomCoeffY1)));
      }
      if (Mov_DiagonalChunkCoord2 == Mov_DiagonalBorderBiom2){
        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(to_string(Scene.coords_biomes[0].x + Mov_BiomCoeffX2) + to_string(Scene.coords_biomes[0].y + Mov_BiomCoeffY2)));
      }
    }

    // if next coordinate x of center chunk placed in next biome zone
    if (Mov_CenterChunkCoord == Mov_BorderBiom){
      // changing coordinates of biome zone
      MoveBiomes(Mov_horizontal, Mov_increase);
    }
    // changing of position and content of loaded into memory chunks
    MoveChunks(Mov_horizontal, Mov_increase);
  }
  // changing camera and tile coordinates
  MoveCamera(Mov_horizontal, Mov_increase);
}

// repainting OpenGL by reshape window
void SceneReshape(GLsizei Width, GLsizei Height){
  Scene.Reshape(Width, Height);
}

// painting Scene

void Draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(Scene.xpos_cam, Scene.ypos_cam, -6.0,
            Scene.xpos_cam, Scene.ypos_cam, 1.0,
            0.0, 1.0, 0.0);

    // painting world
    glScalef(scale, scale, 0.0);

    Scene.DrawChunk(0, Scene.coords_chunks[0].x, Scene.coords_chunks[0].y);
    Scene.DrawChunk(1, Scene.coords_chunks[1].x, Scene.coords_chunks[1].y);
    Scene.DrawChunk(2, Scene.coords_chunks[2].x, Scene.coords_chunks[2].y);
    Scene.DrawChunk(3, Scene.coords_chunks[3].x, Scene.coords_chunks[3].y);
    Scene.DrawChunk(4, Scene.coords_chunks[4].x, Scene.coords_chunks[4].y);
    Scene.DrawChunk(5, Scene.coords_chunks[5].x, Scene.coords_chunks[5].y);
    Scene.DrawChunk(6, Scene.coords_chunks[6].x, Scene.coords_chunks[6].y);
    Scene.DrawChunk(7, Scene.coords_chunks[7].x, Scene.coords_chunks[7].y);
    Scene.DrawChunk(8, Scene.coords_chunks[8].x, Scene.coords_chunks[8].y);

    DrawPlayer();

    glutSwapBuffers();
}

void ConsoleDraw(){
  Console.Draw(console, wnd);
}

void ReshapeConsole(int w, int h){
  glutUseLayer(GLUT_OVERLAY);
  glViewport(0, 0, 640, 480);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 640, 0, 480);
  glScalef(1, -1, 1);
  glTranslatef(0, -480, 0);
  glMatrixMode(GL_MODELVIEW);
  glutUseLayer(GLUT_NORMAL);
}

// processing keys

void Keyboard(unsigned char key, int x, int y) {

  Console.current_key = (char)key;

  switch (key){
  // escape
  case 27:
    Scene.ClearTextures();
    glutDestroyWindow(wnd);
  break;
  // `
  case 96:

  break;
  }
}

// processing keys
void ExtKeyboard(int key, int x, int y){
  switch (key){
    // key left arrow
    case GLUT_KEY_LEFT:
      ProcessMoving(0);
    break;
    // key right arrow
    case GLUT_KEY_RIGHT:
      ProcessMoving(1);
    break;
    // key up arrow
    case GLUT_KEY_UP:
      ProcessMoving(2);
    break;
    // key down arrow
    case GLUT_KEY_DOWN:
      ProcessMoving(3);
    break;
  }
}

void Idle(){
  Draw();
  ConsoleDraw();
}

int main(int argc, char *argv[]) {

    minIni ini = OpenIniFile(mainini);
    string s= getTextureIni(ini);
    vector<string> texturelist= getTextureList(s);

    // getting seed
    // loading world
    /*std::cout << "Enter seed:";
    std::cin >> str_seed;*/
    str_seed = "3214n6kj245nlk6n13n231n5l243n51";

    // initializing and create window GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    //glutInitWindowPosition(0, 0);
    //glutInitWindowSize(640, 480);
    wnd = glutCreateWindow("Evolution 0.1.0");
    glutFullScreen();

    // initializing coordinates of biomes zones
    Scene.coords_biomes[0].x = 0;
    Scene.coords_biomes[0].y = 0;

    // initializing coordinates of chunks
    Scene.coords_chunks[0].x = 0;     Scene.coords_chunks[0].y = 0;
    Scene.coords_chunks[1].x = 0;     Scene.coords_chunks[1].y = 1;
    Scene.coords_chunks[2].x = -1;    Scene.coords_chunks[2].y = 1;
    Scene.coords_chunks[3].x = -1;    Scene.coords_chunks[3].y = 0;
    Scene.coords_chunks[4].x = -1;    Scene.coords_chunks[4].y = -1;
    Scene.coords_chunks[5].x = 0;     Scene.coords_chunks[5].y = -1;
    Scene.coords_chunks[6].x = 1;     Scene.coords_chunks[6].y = -1;
    Scene.coords_chunks[7].x = 1;     Scene.coords_chunks[7].y = 0;
    Scene.coords_chunks[8].x = 1;     Scene.coords_chunks[8].y = 1;

    // coordinates and borders
    Scene.xpos = 0;
    Scene.ypos = 0;
    Scene.border_chunk_xpos = Scene.xpos + (size_zone / 2);
    Scene.border_chunk_ypos = Scene.ypos + (size_zone / 2);
    Scene.border_biom_xpos = Scene.coords_chunks[0].x + (size_zone_biomes / 2);
    Scene.border_biom_ypos = Scene.coords_chunks[0].y + (size_zone_biomes / 2);
    // loading world
    int_seed = StrToInt(str_seed);
    loaded_biomes[0] = LoadBiomes(int_seed, StrToInt("00"));

    Scene.loaded_tiles[0] = LoadChunk(int_seed, StrToInt("00"), GetIDBiome(0, 0, 0));
    Scene.loaded_tiles[1] = LoadChunk(int_seed, StrToInt("01"), GetIDBiome(0, 1, 1));
    Scene.loaded_tiles[2] = LoadChunk(int_seed, StrToInt("-11"), GetIDBiome(-1, 1, 2));
    Scene.loaded_tiles[3] = LoadChunk(int_seed, StrToInt("-10"), GetIDBiome(-1, 0, 3));
    Scene.loaded_tiles[4] = LoadChunk(int_seed, StrToInt("-1-1"), GetIDBiome(-1, -1, 4));
    Scene.loaded_tiles[5] = LoadChunk(int_seed, StrToInt("0-1"), GetIDBiome(0, -1, 5));
    Scene.loaded_tiles[6] = LoadChunk(int_seed, StrToInt("1-1"), GetIDBiome(1, -1, 6));
    Scene.loaded_tiles[7] = LoadChunk(int_seed, StrToInt("10"), GetIDBiome(1, 0, 7));
    Scene.loaded_tiles[8] = LoadChunk(int_seed, StrToInt("11"), GetIDBiome(1, 1, 8));

    // load Textures
    Scene.LoadTextures(texturelist);


    // defining events of window
    glutDisplayFunc(Draw);
    glutReshapeFunc(SceneReshape);
    glutIdleFunc(Idle);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(ExtKeyboard);

    // fon color
    glClearColor(0.5, 0.5, 0.5, 0.0);
    // smoothing paint of color
    glShadeModel(GL_SMOOTH);
    // modificated perspective
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    console = glutCreateSubWindow(wnd, 10, 10, 1024, 200);
    glClearColor(0.0, 1.0, 0.0, 0.0);
    glutDisplayFunc(ConsoleDraw);

    // processing events of window
    glutMainLoop();
    // clear textures
    Scene.ClearTextures();
    glutDestroyWindow(wnd);

    return 0;
}
