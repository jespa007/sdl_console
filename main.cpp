
#include "CConsole.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define SDL_CONSOLE_MAJOR 0
#define SDL_CONSOLE_MINOR 0
#define SDL_CONSOLE_PATCH 1

// uncomment if you want to know the FPS ...
//#define __SHOW_FPS__

/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */



#define RESOLUTION_COLUMNS  10
#define RESOLUTION_ROWS     4




//const char *about="ZetScript 1.2.0 2017 (c) Jordi Espada\n";

void one_iter();

CFont *font=NULL;
CConsole *console=NULL;
bool quit = false;

//#define WIDTH_CHAR  8
//#define HEIGHT_CHAR 16

bool error = false;

int main(int argc, char *argv[]){


	printf("SDL console %02i.%02i.%02i\n",SDL_CONSOLE_MAJOR,SDL_CONSOLE_MINOR,SDL_CONSOLE_PATCH);

	// here we initiate the singletons ...
	/*if (argc < 3) {
		printf("Pass font load and size w.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("sdl_console incoshow.png 16");
		printf("\n");
		printf("\n");
		return 0;
	}*/


	console=CConsole::getInstance();

	console->init(RESOLUTION_COLUMNS*8,RESOLUTION_ROWS*16);
	//--------------------------------------------------
	// 1. Make our C++ bindings for script calls

	// create direct call script function

	font = new CFont();

	//font->load("font.bmp",8,16);
	font->load((const unsigned char **)NULL,16,256);


	console->setFont(font);

#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(one_iter, 0, 1);
#else

	do{


		one_iter();
		// SDL_Delay(16);


	}
	while(!quit);
#endif


	// destroy singletons...
	CInput::destroy();

	return 0;
}

//char currentLine[16382]={0};
//int currentPosX=0;
//char *currentChar=currentLine;






void one_iter(){

	if(!console->update()){
		quit = true;
	}

}
