/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */


#include "CConsole.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#define RESOLUTION_COLUMNS  80
#define RESOLUTION_ROWS     25

void one_iter();

CFont *font=NULL;
CConsole *console=NULL;
bool quit = false;
bool error = false;

int main(int argc, char *argv[]){


	console=CConsole::getInstance();

	console->init(RESOLUTION_COLUMNS,RESOLUTION_ROWS);

	console->printOut("SDL console %02i.%02i.%02i",SDL_CONSOLE_MAJOR,SDL_CONSOLE_MINOR,SDL_CONSOLE_PATCH);
	console->printOut("");


	console->setPrompt("co>");
	//--------------------------------------------------
	// 1. Make our C++ bindings for script calls

	// create direct call script function


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


void one_iter(){

	const char * input = console->update();

	if(input){
		printf("%s\n",input);
	}

	if(console->end()){
		quit = true;
	}
}
