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

/*#######################   classes   #######################*/
class ClassScene{
  private:
    // moving and generate chunks
    // parameters: horizontal moving or not and increase or decrease coordinates
    void MoveChunks(bool horizontal, bool increase);
    // moving camera
    void MoveCamera(bool horizontal, bool increase);
    // moving biome zones
    void MoveBiomes(bool horizontal, bool increase);
  public:
    // count of textures
    unsigned short int count_tex;
    // index texture of tiles
    GLuint *tiles_tex;
    // array for loading textures
    map <string, unsigned short int> TextureManager;

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
    // three biome zones (current vertical, horizontal and diagonal next zone)
    tag_biomes loaded_biomes[4];
    // temporary structure for redefine loaded_biomes
    tag_biomes tmp_biomes[1];

    // parameters for load and generate world with seed
    string str_seed, int_seed;

    // initialization count of textures
    void TexInit(unsigned short int count_tex);
    // loading texture by filename
    void LoadTextureImage(const char *texName, GLuint texture);
    // enter to ortho mode
    void setOrthoProjection(GLsizei Width, GLsizei Height);
    // exit of ortho mode
    void setPerspectiveProjection();
    // loading three chunks
    void LoadThreeChunks(tag_id_chunks id_chunks, tag_coords_chunks coords_chunks_A, tag_coords_chunks coords_chunks_B, tag_coords_chunks coords_chunks_C);
    //drawing chunk
    void DrawChunk(unsigned short int index, int x, int y);
    // output string
    void DrawStaticString(float x, float y, float z, void *font, string input);
    //drawing player
    void DrawPlayer();
    // repainting OpenGL by reshape window
    void Reshape(GLsizei Width, GLsizei Height);
    // loading textures
    void LoadTextures(vector<string> texturelist);
    // deleting textures
    void ClearTextures();
    // painting Scene
    void Draw();
    /*processing move camera
      variable direction:
      0 - left
      1 - right
      2 - up
      3 - down*/
    void ProcessMoving(unsigned short int direction);
    // function for getting id biome from array using on input coordinates of chunk
    unsigned short int GetIDBiome(int x, int y, unsigned short int id_chunk);
    // destructor
    ~ClassScene();

    // friendly classes
    friend class ClassConsole;
};

class ClassConsole{
  public:
    // fps calculating parameters
    int fps;
    long t, dt;
    string fps_str;
    // pressed key
    string current_key;
    // console visible state
    bool visible = false;

    // painting Console
    void Draw(ClassScene &object, unsigned short int console, unsigned short int wnd);
    // set 2d mode
    void Reshape(GLsizei Width, GLsizei Height);
};

#endif
