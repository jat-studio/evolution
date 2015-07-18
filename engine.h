using namespace std;
#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED


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

/*#######################   classes   #######################*/
class ClassScene{
  public:
    static const unsigned short int count_tex = 6; // count of textures
    GLuint tiles_tex[count_tex]; // index texture of tiles
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
    // loaded into memory tiles
    tag_tiles loaded_tiles[num_zones];

    // loading texture by filename
    void LoadTextureImage(const char *texName, GLuint texture);
    // enter to ortho mode
    void setOrthographicProjection();
    // exit of ortho mode
    void setPerspectiveProjection();
    //drawing chunk
    void DrawChunk(unsigned short int index, int x, int y);
    // output string
    void DrawStaticString(float x, float y, float z, void *font, string input);
    // repainting OpenGL by reshape window
    void Reshape(GLsizei Width, GLsizei Height);
    // loading textures
    void LoadTextures(vector<string> texturelist);
    // deleting textures
    void ClearTextures();
};

class ClassConsole: public ClassScene{
  public:
    int fps;
    long t, dt;
    string fps_str;
    string current_key;

    // painting Console
    void Draw(unsigned short int console, unsigned short int wnd);
};

#endif
