/* 
 * File:   config.h
 * Author: shadowmaster63
 *
 * Created on 8 июля 2015 г., 9:42
 */
#include "minini/minIni.h"
#ifndef CONFIG_H
#define	CONFIG_H

const std::string mainini = "evolution.ini";
minIni OpenIniFile(const std::string mainini);
std::string getTextureIni(minIni ini);
void getTextureList(std::string ini);
#endif	/* CONFIG_H */

