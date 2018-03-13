/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "CInput.h"
#include <cstring>

CInput *CInput::input=NULL;




CInput * CInput::getInstance(){
	if(input == NULL){
		input = new CInput();
	}
	return input;
}

void CInput::destroy(){
	if(input){
		delete input;
	}
	input = NULL;
}

CInput::CInput(){
	buffer=NULL;
}

SDL_Event * CInput::update(){

	memset(key,0,sizeof(key));
	Uint32 m_idxKey=0;
	tEventKey the_keyEvent;


	if( SDL_PollEvent( &Event ) )
	{
		memset(&the_keyEvent,0,sizeof(the_keyEvent));

		if(Event.key.repeat == 0){ // one key key press at time...

			switch(Event.type) {
			//case SDL_KEYUP:
			case SDL_KEYDOWN:
				return &Event;
				/*m_idxKey = Event.key.keysym.sym;

				//print_info_cr("UnPress");
				switch(Event.key.keysym.sym){


					case SDLK_UP: m_idxKey = KEY_UP; break;
					case SDLK_DOWN: m_idxKey = KEY_DOWN; break;
					case SDLK_LEFT: m_idxKey = KEY_LEFT; break;
					case SDLK_RIGHT:m_idxKey = KEY_RIGHT; break;
					case SDLK_ESCAPE: m_idxKey = KEY_ESCAPE;  break;
					case SDLK_F5: m_idxKey = KEY_F5; break;
					case SDLK_F9: m_idxKey = KEY_F9;break;
					case SDLK_BACKSPACE:m_idxKey=KEY_SPACE;break;

					default:
						if(KEY_SPACE <= Event.key.keysym.sym && Event.key.keysym.sym <= KEY_z) {
							the_keyEvent.codeKey = Event.key.keysym.sym;
						}

						break;
				}

				if(m_idxKey<KEY_LAST) {

					//keyR[m_idxKey]=false;


						key[m_idxKey]=true;
						//keyR[m_idxKey]=true;




				}*/

				break;

			}
		}
	}

	return NULL;
}
