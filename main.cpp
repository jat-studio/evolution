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
#include "strings.h"

/*Unix environment*/
//#include <unistd.h>
//#include <linux/limits.h>
//#include <typeinfo>

// id GLwindow
unsigned short int wnd, console;

// application mode:
// 0 - game
// 1 - console
unsigned short int AppMode = 0;

// list of textures
vector<string> texturelist;

// Class for basic functions
ClassScene Scene;
// Class for console
ClassConsole Console;

//return path to executing program
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

// processing keys in game mode
void GameModeKeys(unsigned char key){
  switch (key){
    // escape - exit
    case 27:
      Scene.ClearTextures();
      glutDestroyWindow(wnd);
    break;
    // "`" key - enter console mode
    case 96:
      Console.visible = true;
      AppMode = 1;
    break;
  }
}

// processing keys in game mode
void ConsoleModeKeys(unsigned char key){
  // escape - exit to game mode
  if (key == 27){
    Console.visible = false;
    Console.current_key = "Command~:";
    AppMode = 0;
  }
  //Console.current_key = Int_To_Str((int)key);
  // saving pressed keys to current_key variable if key is valid
  if ((key >= 33) && (key <= 126)){
    Console.current_key += key;
  }
  // pressing enter
  if (key == 13){
    Console.Enter();
  }
}

// processing keys
void Keyboard(unsigned char key, int x, int y){
  // select application mode
  switch (AppMode){
    // game mode
    case 0:
      GameModeKeys(key);
    break;
    // console mode
    case 1:
      ConsoleModeKeys(key);
    break;
  }
}

// processing arrow keys
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
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(1024, 768);
  wnd = glutCreateWindow("Evolution 0.1.0");
  //glutFullScreen();

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
  Scene.int_seed = Str_To_Char_Codes(Scene.str_seed);
  Scene.loaded_biomes[0] = LoadBiomes(Scene.int_seed, Str_To_Char_Codes("00"));

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

  // background color
  glClearColor(0.5, 0.5, 0.5, 0.0);
  // smoothing paint of color
  glShadeModel(GL_SMOOTH);
  // modified perspective
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  console = glutCreateSubWindow(wnd, 10, 10, glutGet(GLUT_WINDOW_WIDTH) - 20, 200);
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
