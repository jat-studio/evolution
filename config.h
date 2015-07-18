using namespace std;
/*
 * File:   config.h
 * Author: shadowmaster63
 *
 * Created on 8 июля 2015 г., 9:42
 */
#include "minini/minIni.h"
#include <vector>

#ifndef CONFIG_H
#define	CONFIG_H

const string mainini = "evolution.ini";
minIni OpenIniFile(const std::string mainini);
std::string getTextureIni(minIni ini);
std::vector<std::string> getTextureList(std::string ini);
int getTotalSections(minIni ini);
int getTotalKeys(minIni ini,std::string section);
#endif	/* CONFIG_H */

