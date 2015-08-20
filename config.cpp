using namespace std;
/*Basic c library*/
#include <iostream>
#include <vector>

/*My library*/
#include "minini/minIni.h"

//Открытие ini файлов
minIni OpenIniFile(const string mainini) {
    minIni ini(mainini);
    return ini;
}

string getTextureIni(minIni ini) {
    string s;
    s = ini.gets("config_path", "texture", "NULL");
    return s;
}

//количество секций в конфиге
int getTotalSections(minIni ini){
    int is;
    string section;
    for (is = 0; section = ini.getsection(is), section.length() > 0; is++) {}
    return is;
}

//количество ключей в секции
int getTotalKeys(minIni ini,string section){
    int ik;
    string s;
    for (ik = 0; s = ini.getkey(section, ik), s.length() > 0; ik++) {}
    return ik;
}

//список текстур
vector<string> getTextureList(string ini){
  minIni texture = OpenIniFile(ini);
  string section, s, value;
  vector<string> textures;
  value = texture.gets("path", "path", "NULL");
  textures.insert(textures.end(), value);
  for (int is = 1; section = texture.getsection(is), section.length() > 0; is++){
    textures.reserve(getTotalKeys(texture, section));
    for (int ik = 0; s = texture.getkey(section, ik), s.length() > 0; ik++){
      textures.insert(textures.end(), s);
      value = texture.gets(section, s, "NULL");
      textures.insert(textures.end(), value);
    }
  }
  return textures;
}
