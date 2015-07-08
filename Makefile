## -*- Makefile -*-
##
## Пользователь: shadowmaster63
## Время: 07.07.2015 15:16:47
## Makefile created by Oracle Solaris Studio.
##
## Этот файл создан автоматически.
##


#### Определения компиляторов и средств, используемые совместно всеми целями сборки #####
CCC = gcc
CXX = g++
BASICOPTS = -g
CXXFLAGS = $(BASICOPTS) -c -std=c++11 -Wall
CCFLAGS = $(BASICOPTS) -c
CCADMIN = 


# Определите целевые каталоги.
TARGETDIR_evolution=bin


all: $(TARGETDIR_evolution)/evolution

## Цель: evolution
OBJS_evolution = $(TARGETDIR_evolution)/main.o \
		 $(TARGETDIR_evolution)/engine.o \
		 $(TARGETDIR_evolution)/worldgen.o \
		 $(TARGETDIR_evolution)/config.o \
		 $(TARGETDIR_evolution)/minIni.o
USERLIBS_evolution = $(SYSLIBS_evolution) 


INCS=-I/usr/include
LIBS=-L/usr/include -lGL -lGLU -lglut -lIL -lILU 
# Скомпилировать исходные файлы .o

$(TARGETDIR_evolution)/main.o: main.cpp
	$(CXX)  $(CXXFLAGS) $(INCS) -o $@ main.cpp
$(TARGETDIR_evolution)/engine.o: engine.cpp
	$(CXX)  $(CXXFLAGS) $(INCS) -o $@ engine.cpp
$(TARGETDIR_evolution)/worldgen.o: worldgen.cpp
	$(CXX)  $(CXXFLAGS) $(INCS) -o $@ worldgen.cpp
$(TARGETDIR_evolution)/config.o: config.cpp
	$(CXX)  $(CXXFLAGS) $(INCS) -o $@ config.cpp
$(TARGETDIR_evolution)/minIni.o: minini/minIni.c
	$(CXX)  $(CCFLAGS) $(INCS) -o $@ minini/minIni.c
# Исполняемый файл
$(TARGETDIR_evolution)/evolution: $(TARGETDIR_evolution) $(OBJS_evolution)
	$(CXX) $(CXXFLAGS_evolution) $(OBJS_evolution) -o $@  $(LIBS) 



#### При выполнении команды "Очистить цель" будут удалены все созданные файлы ####
clean:
	rm -f \
		$(TARGETDIR_evolution)/evolution \
		$(TARGETDIR_evolution)/main.o	\
		$(TARGETDIR_evolution)/engine.o	\
		$(TARGETDIR_evolution)/worldgen.o	\
		$(TARGETDIR_evolution)/config.o	    \
		$(TARGETDIR_evolution)/minIni.o
	$(CCADMIN)
	rm -f -r $(TARGETDIR_evolution)


# Создать целевой каталог (при необходимости)
$(TARGETDIR_evolution):
	mkdir -p $(TARGETDIR_evolution)


# Включить проверку зависимостей
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux
