/*Basic c library*/
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <array>

/*Open GL*/
#include "GL/glut.h"
#include "IL/il.h"
#include "IL/ilu.h"

/*My library*/
#include "worldgen.h"
#include "engine.h"

/*Unix envirement*/
#include <unistd.h>
#include <linux/limits.h>
#include <typeinfo>

/*Config ini function*/
#include "config.h"
#include "minini/minIni.h"
#include <vector>

unsigned short int wnd; // id GLwindow

const unsigned short int count_tex = 5; // count of textures
GLuint tiles_tex[count_tex]; // index texture of tiles

// fps drawing
int fps;
long t, dt;
std::string fps_str;

// parameters for load and generate world with seed
tag_biomes loaded_biomes[4]; // three biome zones (current vertical, horizontal and diagonal next zone)
tag_biomes tmp_biomes[1]; // temporary structure for redefine loaded_biomes
tag_tiles loaded_tiles[num_zones];
std::string str_seed, int_seed;

// coordinates of camera
float xpos_cam = 0.0;
float ypos_cam = 0.0;

// coordinates of tiles
int xpos;
int ypos;

// coordinates of left and top border chunks
int border_chunk_xpos;
int border_chunk_ypos;

// coordinates of left and top border biomes
int border_biom_xpos;
int border_biom_ypos;

// structure for id chunks
tag_id_chunks id_chunks;
// structure for int coordinates of biomes zones
tag_coords_biomes coords_biomes[1];
// structure for int coordinates of chunks
tag_coords_chunks coords_chunks[9];
// Class for basic functions
ClassScene Scene;

//return path to executing programm

char* getPath() {
    char path[PATH_MAX];
    int cnt;
    if ((cnt = readlink("/proc/self/exe", path, PATH_MAX)) > 0) {
        path[cnt] = 0;
    }
    char* getpath = (char *) malloc(sizeof (char) * (cnt + 1));
    std::strncpy(getpath, path, cnt);
    getpath[cnt + 1] = 0;
    return getpath;
}

// loading textures

void LoadTextures(std::vector<std::string> texturelist) {
    // create array of textures
    glGenTextures(count_tex, &tiles_tex[0]);

    // initializing il and ilu library
    ilInit();
    iluInit();
    // loading textures
    for(uint i=0;i<texturelist.size();i++){
      Scene.LoadTextureImage(texturelist[i].c_str(), tiles_tex[i]);  
    }
    // enabling textures
    glEnable(GL_TEXTURE_2D);
}

// deleting textures

void clearTextures() {
    glDeleteTextures(count_tex, &tiles_tex[0]);
}

//drawing chunk

void DrawChunk(unsigned short int index, int x, int y) {
    unsigned short int loop_col, loop_row, tile_get;

    glPushMatrix();
    glTranslatef(x * size_zone, y * size_zone, 0.0);
    for (loop_col = 0; loop_col < size_zone; loop_col++) {
        for (loop_row = 0; loop_row < size_zone; loop_row++) {
            tile_get = loaded_tiles[index].tile_id [loop_col] [loop_row];

            glBindTexture(GL_TEXTURE_2D, tiles_tex[tile_get]);

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

//drawing player

void DrawPlayer() {
    glPushMatrix();
    glTranslatef(xpos, ypos, 0.0);
    glBindTexture(GL_TEXTURE_2D, tiles_tex[4]);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0, 1.0, -0.01);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(-1.0, 1.0, -0.01);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(-1.0, 0.0, -0.01);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.0, 0.0, -0.01);
    glEnd();
    glPopMatrix();
}

// function for getting id biome from array using on input coordinates of chunk

unsigned short int GetIDBiome(int x, int y) {
    /*Необходимо выдавать на основе координат чанков порядковые номера в массивах*/

    // change of coordinates for convenience
    short int coeff_x = x + (size_zone_biomes / 2) - 1;
    short int coeff_y = y + (size_zone_biomes / 2) - 1;
    // if coordinate x ranges in array loaded_biomes[0]
    if ((coeff_x >= 0) and (coeff_x < size_zone_biomes)) {
        // if coordinate y ranges in array loaded_biomes[0]
        if ((coeff_y >= 0) and (coeff_y < size_zone_biomes)) {
            x = coeff_x;
            y = coeff_y;
            return loaded_biomes[0].biome_id[x][y];
        }// if coordinate y has run out of array loaded_biomes[0]
        else {
            // redefining y coordinate of chunk to id next array
            if (coeff_y < 0) {
                y = coeff_x + size_zone_biomes;
            } else {
                y = coeff_y - size_zone_biomes;
            }
            x = coeff_x;
            return loaded_biomes[2].biome_id[x][y];
        }
    }// if coordinate x has run out of array loaded_biomes[0]
    else {
        // redefining x coordinate of chunk to id next array
        if (coeff_x < 0) {
            x = coeff_x + size_zone_biomes;
        } else {
            x = coeff_x - size_zone_biomes;
        }
        y = coeff_y;
        return loaded_biomes[1].biome_id[x][y];
    }
}

// loading three chunks

void LoadThreeChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks_A, tag_coords_chunks coords_chunks_B, tag_coords_chunks coords_chunks_C) {
    loaded_tiles[id_chunks.ida] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_A.x) + std::to_string(coords_chunks_A.y)), GetIDBiome(coords_chunks_A.x, coords_chunks_A.y));
    loaded_tiles[id_chunks.idb] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_B.x) + std::to_string(coords_chunks_B.y)), GetIDBiome(coords_chunks_B.x, coords_chunks_B.y));
    loaded_tiles[id_chunks.idc] = LoadChunk(int_seed, StrToInt(std::to_string(coords_chunks_C.x) + std::to_string(coords_chunks_C.y)), GetIDBiome(coords_chunks_C.x, coords_chunks_C.y));
}

// moving and generate chunks
// parameters: horizontal moving or not and increase or decrease coordinates

void MoveChunks(bool horizontal, bool increase) {
    // redefining chunk id and coordinates
    std::array<unsigned short int, 9> id;
    short int coeff = 0;
    if (horizontal) {
        if (increase) {
            // left
            id = {2, 3, 4, 1, 0, 5, 8, 7, 6};
            coeff = 1;
        } else {
            // right
            id = {8, 7, 6, 1, 0, 5, 2, 3, 4};
            coeff = -1;
        }
        coords_chunks[0].x += coeff;
        coords_chunks[1].x += coeff;
        coords_chunks[2].x += coeff;
        coords_chunks[3].x += coeff;
        coords_chunks[4].x += coeff;
        coords_chunks[5].x += coeff;
        coords_chunks[6].x += coeff;
        coords_chunks[7].x += coeff;
        coords_chunks[8].x += coeff;
    } else {
        if (increase) {
            // up
            id = {6, 5, 4, 7, 0, 3, 8, 1, 2};
            coeff = 1;
        } else {
            // down
            id = {8, 1, 2, 7, 0, 3, 6, 5, 4};
            coeff = -1;
        }
        coords_chunks[0].y += coeff;
        coords_chunks[1].y += coeff;
        coords_chunks[2].y += coeff;
        coords_chunks[3].y += coeff;
        coords_chunks[4].y += coeff;
        coords_chunks[5].y += coeff;
        coords_chunks[6].y += coeff;
        coords_chunks[7].y += coeff;
        coords_chunks[8].y += coeff;
    }

    // redefining content of chunks
    loaded_tiles[id[0]] = loaded_tiles[id[3]];
    loaded_tiles[id[1]] = loaded_tiles[id[4]];
    loaded_tiles[id[2]] = loaded_tiles[id[5]];

    loaded_tiles[id[3]] = loaded_tiles[id[6]];
    loaded_tiles[id[4]] = loaded_tiles[id[7]];
    loaded_tiles[id[5]] = loaded_tiles[id[8]];

    // generate three new chunks
    LoadThreeChunks({id[6], id[7], id[8]},
    {
        coords_chunks[id[6]].x, coords_chunks[id[6]].y
    },
    {
        coords_chunks[id[7]].x, coords_chunks[id[7]].y
    },
    {
        coords_chunks[id[8]].x, coords_chunks[id[8]].y
    });
}

// processing move camera
// variable direction:
// 0 - left
// 1 - right
// 2 - up
// 3 - down
/*void ProcessMoving(unsigned short int direction){
  // defining variables
  int Mov_pos;
  int Mov_border;
  int Mov_BorderBiom;
  bool Mov_increase;
  bool Mov_horizontal;
  int Mov_BorderChunkCoord;
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
      Mov_pos = xpos + 1;
      Mov_border = border_chunk_xpos + 1;
      Mov_BorderBiom = border_biom_xpos + 1;
      Mov_increase = true;
      Mov_horizontal = true;
      Mov_BorderChunkCoord = coords_chunks[7].x + 1;
      Mov_DiagonalChunkCoord1 = coords_chunks[1].y + 1;
      Mov_DiagonalChunkCoord2 = coords_chunks[5].y - 1;
      Mov_DiagonalBorderBiom1 = border_biom_ypos + 1;
      Mov_DiagonalBorderBiom2 = border_biom_ypos - size_zone_biomes;
      Mov_IDBiomesArray = 1;
      Mov_CoeffX = 1;
      Mov_CoeffY = 0;
      Mov_BiomCoeffX1 = 1;
      Mov_BiomCoeffY1 = 1;
      Mov_BiomCoeffX2 = 1;
      Mov_BiomCoeffY2 = -1;

    break;
  }

  // if next coordinate placed in next chunk
  if (Mov_pos == Mov_border){
    // if next coordinate of border chunk placed in next biome zone
    if (Mov_BorderChunkCoord == Mov_BorderBiom){
      // load next biome zone
      loaded_biomes[Mov_IDBiomesArray] =
      LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x + Mov_CoeffX)
                 + std::to_string(coords_biomes[0].y + Mov_CoeffY)));
      // load next diagonal biome zone ([3])
      if (Mov_DiagonalChunkCoord1 == Mov_DiagonalBorderBiom1){
        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x + Mov_BiomCoeffX1) + std::to_string(coords_biomes[0].y + Mov_BiomCoeffY1)));
      }
      if (Mov_DiagonalChunkCoord2 == Mov_DiagonalBorderBiom2){
        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x + Mov_BiomCoeffX2) + std::to_string(coords_biomes[0].y + Mov_BiomCoeffY2)));
      }
    }

        // if next coordinate x of center chunk placed in next biome zone
        if ((coords_chunks[0].x + Mov_CoeffX) == ()){
          // increase coordinates of biome zone
          coords_biomes[0].x++;
          // redefine biome zones
          tmp_biomes[0] = loaded_biomes[0];
          loaded_biomes[0] = loaded_biomes[1];
          loaded_biomes[1] = tmp_biomes[0];
        }

        // changing of position and content of loaded into memory chunks
        MoveChunks(true, true);
      }
      // increase camera and tile coordinates
      xpos_cam += scale;
      xpos++;
}*/

// repainting OpenGL by reshape window

void Reshape(GLsizei Width, GLsizei Height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glViewport(0, 0, Width, Height);
    gluPerspective(45.0, (GLfloat) Width / (GLfloat) Height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// painting Scene

void Draw() {
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



    // painting text on 2d mode
    Scene.setOrthographicProjection();
    // drawing fps
    fps++;
    t = glutGet(GLUT_ELAPSED_TIME);
    if (t - dt > 1000) {
        fps_str = "FPS:" + std::to_string((int) (fps * 1000.0 / (t - dt)));
        dt = t;
        fps = 0;
    }
    Scene.DrawStaticString(-0.99, 0.95, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, fps_str);

    // drawing coordinates
    std::string print_str;
    print_str = "X: ";
    print_str += std::to_string(xpos);
    print_str += "; Y: ";
    print_str += std::to_string(ypos);
    Scene.DrawStaticString(-0.99, 0.91, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str);

    // drawing coordinates chunks
    std::string print_str1;
    print_str1 = "Xc: ";
    print_str1 += std::to_string(coords_chunks[0].x);
    print_str1 += "; Yc: ";
    print_str1 += std::to_string(coords_chunks[0].y);
    Scene.DrawStaticString(-0.99, 0.87, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str1);

    // drawing coordinates biomes
    std::string print_str2;
    print_str2 = "Xb: ";
    print_str2 += std::to_string(coords_biomes[0].x);
    print_str2 += "; Yb: ";
    print_str2 += std::to_string(coords_biomes[0].y);
    Scene.DrawStaticString(-0.99, 0.83, 0.0, GLUT_BITMAP_TIMES_ROMAN_24, print_str2);

    Scene.setPerspectiveProjection();

    glutSwapBuffers();
}

// processing keys

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
            // escape
        case 27:
            clearTextures();
            glutDestroyWindow(wnd);
            break;
    }
}

// processing keys

void ExtKeyboard(int key, int x, int y) {
    switch (key) {
            // key left arrow
        case GLUT_KEY_LEFT:
            // if next coordinate x placed in next chunk
            if (CalculateBorder(xpos, size_zone, true)) {

                // if next coordinate x of left chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[7].x, size_zone_biomes, true)) {
                    // load next horizontal biome zone ([1])
                    loaded_biomes[1] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x + 1) + std::to_string(coords_biomes[0].y)));
                    // load next diagonal biome zone ([3])
                    if (CalculateBorder(coords_chunks[7].x, size_zone_biomes, true)) {
                        loaded_biomes[3] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
                    }
                }

                // if next coordinate x of center chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[0].x, size_zone_biomes, true)) {
                    // increase coordinates of biome zone
                    coords_biomes[0].x++;
                    // redefine biome zones
                    tmp_biomes[0] = loaded_biomes[0];
                    loaded_biomes[0] = loaded_biomes[1];
                    loaded_biomes[1] = tmp_biomes[0];
                }

                // changing of position and content of loaded into memory chunks
                MoveChunks(true, true);
            }
            // increase camera and tile coordinates
            xpos_cam += scale;
            xpos++;
            break;

            // key right arrow
        case GLUT_KEY_RIGHT:
            // if next coordinate x placed in next chunk
            if (CalculateBorder(xpos, size_zone, false)) {

                // if next coordinate x of left chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[3].x, size_zone_biomes, false)) {
                    // load next horizontal biome zone ([1])
                    loaded_biomes[1] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
                }

                // if next coordinate x of center chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[0].x, size_zone_biomes, false)) {
                    // increase coordinates of biome zone
                    coords_biomes[0].x--;
                    // redefine biome zones
                    tmp_biomes[0] = loaded_biomes[0];
                    loaded_biomes[0] = loaded_biomes[1];
                    loaded_biomes[1] = tmp_biomes[0];
                }

                // changing of position and content of loaded into memory chunks
                MoveChunks(true, false);
            }
            // increase camera and tile coordinates
            xpos_cam -= scale;
            xpos--;
            break;

            // key up arrow
        case GLUT_KEY_UP:
            // if next coordinate y placed in next chunk
            if (CalculateBorder(ypos, size_zone, true)) {

                // if next coordinate y of up chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[1].y, size_zone_biomes, true)) {
                    // load next vertical biome zone ([2])
                    loaded_biomes[2] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
                }

                // if next coordinate y of center chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[0].y, size_zone_biomes, true)) {
                    // increase coordinates of biome zone
                    coords_biomes[0].y++;
                    // redefine biome zones
                    tmp_biomes[0] = loaded_biomes[1];
                    loaded_biomes[1] = loaded_biomes[2];
                    loaded_biomes[2] = tmp_biomes[0];
                }

                // changing of position and content of loaded into memory chunks
                MoveChunks(false, true);
            }
            // increase camera and tile coordinates
            ypos_cam += scale;
            ypos++;
            break;

            // key down arrow
        case GLUT_KEY_DOWN:
            // if next coordinate y placed in next chunk
            if (CalculateBorder(ypos, size_zone, false)) {

                // if next coordinate y of down chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[5].y, size_zone_biomes, false)) {
                    // load next vertical biome zone ([2])
                    loaded_biomes[2] = LoadBiomes(int_seed, StrToInt(std::to_string(coords_biomes[0].x) + std::to_string(coords_biomes[0].y)));
                }

                // if next coordinate y of center chunk placed in next biome zone
                if (CalculateBorder(coords_chunks[0].y, size_zone_biomes, false)) {
                    // increase coordinates of biome zone
                    coords_biomes[0].y--;
                    // redefine biome zones
                    tmp_biomes[0] = loaded_biomes[1];
                    loaded_biomes[1] = loaded_biomes[2];
                    loaded_biomes[2] = tmp_biomes[0];
                }

                // changing of position and content of loaded into memory chunks
                MoveChunks(false, false);
            }
            // increase camera and tile coordinates
            ypos_cam -= scale;
            ypos--;
            break;
    }
}

void Idle() {
}

int main(int argc, char *argv[]) {

    minIni ini = OpenIniFile(mainini);
    std::string s= getTextureIni(ini);
    std::vector<std::string> texturelist= getTextureList(s);

    
    // getting seed
    // loading world
    std::cout << "Enter seed:";
    std::cin >> str_seed;

    // initializing and create window GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(640, 480);
    wnd = glutCreateWindow("Evolution 0.0.8");

    // initializing coordinates of biomes zones
    coords_biomes[0].x = 0;
    coords_biomes[0].y = 0;

    // initializing coordinates of chunks
    coords_chunks[0].x = 0;
    coords_chunks[0].y = 0;
    coords_chunks[1].x = 0;
    coords_chunks[1].y = 1;
    coords_chunks[2].x = -1;
    coords_chunks[2].y = 1;
    coords_chunks[3].x = -1;
    coords_chunks[3].y = 0;
    coords_chunks[4].x = -1;
    coords_chunks[4].y = -1;
    coords_chunks[5].x = 0;
    coords_chunks[5].y = -1;
    coords_chunks[6].x = 1;
    coords_chunks[6].y = -1;
    coords_chunks[7].x = 1;
    coords_chunks[7].y = 0;
    coords_chunks[8].x = 1;
    coords_chunks[8].y = 1;

    // coordinates and borders
    xpos = 0;
    ypos = 0;
    border_chunk_xpos = xpos + (size_zone / 2);
    border_chunk_ypos = ypos + (size_zone / 2);
    border_biom_xpos = coords_chunks[0].x + (size_zone_biomes / 2);
    border_biom_ypos = coords_chunks[0].y + (size_zone_biomes / 2);
    // loading world
    int_seed = StrToInt(str_seed);
    loaded_biomes[0] = LoadBiomes(int_seed, StrToInt("00"));
    LoadThreeChunks({0, 1, 2},
    {
        0, 0
    },
    {
        0, 1
    },
    {
        1, 1
    });
    LoadThreeChunks({3, 4, 5},
    {
        1, 0
    },
    {
        1, -1
    },
    {
        0, -1
    });
    LoadThreeChunks({6, 7, 8},
    {
        -1, -1
    },
    {
        -1, 0
    },
    {
        -1, 1
    });

    // load Textures
    LoadTextures(texturelist);

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
