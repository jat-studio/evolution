#include "minini/minIni.h"
#include <iostream>
#include <vector>
minIni OpenIniFile(const std::string mainini) {
    minIni ini(mainini);
    return ini;
}

std::string getTextureIni(minIni ini) {
    std::string s;
    s = ini.gets("config_path", "texture", "NULL");
    return s;
}

int getTotalSections(minIni ini){
    int is;
    std::string section;
    for (is = 0; section = ini.getsection(is), section.length() > 0; is++) {}
    return is;
}
int getTotalKeys(minIni ini,std::string section){
    int ik;
    std::string s;
    for (ik = 0; s = ini.getkey(section, ik), s.length() > 0; ik++) {}
    return ik;
}


std::vector<std::string> getTextureList(std::string ini){
  minIni texture = OpenIniFile(ini);
  std::string section,s,value;
  std::vector<std::string> textures;
  for (int is = 0; section = texture.getsection(is), section.length() > 0; is++) {
      textures.reserve(getTotalKeys(texture,section));
    for (int ik = 0; s = texture.getkey(section, ik), s.length() > 0; ik++) {
        value= texture.gets(section,s,"NULL");
        textures.insert(textures.end(),value);
    }
  }
  return textures;   
}