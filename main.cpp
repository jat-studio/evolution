using namespace std;
/*Basic c library*/
#include <cstring>
#include <iostream>
#include <vector>
#include <map>

/*Open GL*/
#include "GL/glut.h"
#include "IL/il.h"
#include "IL/ilu.h"

/*Config ini function*/
#include "config.h"
#include "minini/minIni.h"

/*My library*/
#include "worldgen.h"
#include "engine.h"

/*Unix environment*/
//#include <unistd.h>
//#include <linux/limits.h>
//#include <typeinfo>

// id GLwindow
unsigned short int wnd, console;

// list of textures
vector<string> texturelist;

// Class for basic functions
ClassScene Scene;
// Class for console
ClassConsole Console;

//return path to executing programm
/*char* getPath(){
    char path[PATH_MAX];
    int cnt;
    if ((cnt = readlink("/proc/self/exe", path, PATH_MAX)) > 0) {
        path[cnt] = 0;
    }
    char* getpath = (char *) malloc(sizeof (char) * (cnt + 1));
    strncpy(getpath, path, cnt);
    getpath[cnt + 1] = 0;
    return getpath;
}*/

// repainting OpenGL by reshape window
void SceneReshape(GLsizei Width, GLsizei Height){
  Scene.Reshape(Width, Height);
}

// painting Scene
void SceneDraw(){
  Scene.Draw();
}

// repainting OpenGL by reshape window
void ConsoleReshape(GLsizei Width, GLsizei Height){
  Console.Reshape(Width, Height);
}

// painting Console
void ConsoleDraw(){
  Console.Draw(Scene, console, wnd);
}

// processing keys
void Keyboard(unsigned char key, int x, int y) {

  Console.current_key = key;

  switch (key){
  // escape
  case 27:
    Scene.ClearTextures();
    glutDestroyWindow(wnd);
  break;
  // "`" key
  case 96:
    if (Console.visible){
      Console.visible = false;
    }
    else{
      Console.visible = true;
    }
  break;
  }
}

// processing keys
void ExtKeyboard(int key, int x, int y){
  switch (key){
    // key left arrow
    case GLUT_KEY_LEFT:
      Scene.ProcessMoving(0);
    break;
    // key right arrow
    case GLUT_KEY_RIGHT:
      Scene.ProcessMoving(1);
    break;
    // key up arrow
    case GLUT_KEY_UP:
      Scene.ProcessMoving(2);
    break;
    // key down arrow
    case GLUT_KEY_DOWN:
      Scene.ProcessMoving(3);
    break;
  }
}

void Idle(){
  SceneDraw();
  ConsoleDraw();
}

int main(int argc, char *argv[]){

  minIni ini = OpenIniFile(mainini);
  string s = getTextureIni(ini);
  texturelist = getTextureList(s);

  // getting seed
  // loading world
  /*std::cout << "Enter seed:";
    std::cin >> str_seed;*/
  Scene.str_seed = "3214n6kj245nlk6n13n231n5l243n51";

  // initializing and create window GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  //glutInitWindowPosition(0, 0);
  //glutInitWindowSize(640, 480);
  wnd = glutCreateWindow("Evolution 0.1.0");
  glutFullScreen();

  // initializing textures
  Scene.TexInit(texturelist.size());

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
  Scene.int_seed = StrToInt(Scene.str_seed);
  Scene.loaded_biomes[0] = LoadBiomes(Scene.int_seed, StrToInt("00"));

  /*Scene.loaded_tiles[0] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("00"), Scene.GetIDBiome(0, 0, 0));
  Scene.loaded_tiles[1] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("01"), Scene.GetIDBiome(0, 1, 1));
  Scene.loaded_tiles[2] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("-11"), Scene.GetIDBiome(-1, 1, 2));
  Scene.loaded_tiles[3] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("-10"), Scene.GetIDBiome(-1, 0, 3));
  Scene.loaded_tiles[4] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("-1-1"), Scene.GetIDBiome(-1, -1, 4));
  Scene.loaded_tiles[5] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("0-1"), Scene.GetIDBiome(0, -1, 5));
  Scene.loaded_tiles[6] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("1-1"), Scene.GetIDBiome(1, -1, 6));
  Scene.loaded_tiles[7] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("10"), Scene.GetIDBiome(1, 0, 7));
  Scene.loaded_tiles[8] = LoadChunk(Scene.TextureManager, Scene.int_seed, StrToInt("11"), Scene.GetIDBiome(1, 1, 8));*/

  // load Textures
  Scene.LoadTextures(texturelist);

  // loading starting chunks
  Scene.LoadThreeChunks({0, 1, 2}, {0, 0}, {0, 1}, {-1, 1});
  Scene.LoadThreeChunks({3, 4, 5}, {-1, 0}, {-1, -1}, {0, -1});
  Scene.LoadThreeChunks({6, 7, 8}, {1, -1}, {1, 0}, {1, 1});

  // defining events of window
  glutDisplayFunc(SceneDraw);
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
  glutReshapeFunc(ConsoleReshape);

  // processing events of window
  glutMainLoop();
  // clear textures
  Scene.ClearTextures();
  glutDestroyWindow(wnd);

  return 0;
}
