#include "minini/minIni.h"
#include <iostream>
minIni OpenIniFile(const std::string mainini) {
    minIni ini(mainini);
    return ini;
}

std::string getTextureIni(minIni ini) {
    std::string s;
    s = ini.gets("config_path", "texture", "NULL");
    return s;
}

void getTextureList(std::string ini){
  minIni texture = OpenIniFile(ini);
     std::string section,s;
  for (int is = 0; section = texture.getsection(is), section.length() > 0; is++) {
    std::cout << "[" << section.c_str() << "]" << std::endl;
    for (int ik = 0; s = texture.getkey(section, ik), s.length() > 0; ik++) {
      std::cout << s << std::endl;
    }
  }
     
}