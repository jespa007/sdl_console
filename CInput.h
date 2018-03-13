/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include   <SDL2/SDL.h>
#include   "def_keys.h"

#define  T_ESC	      	CInput::getInstance()->key[KEY_ESCAPE]
#define  T_F5	      	CInput::getInstance()->key[KEY_F5]
#define  T_F9	      	CInput::getInstance()->key[KEY_F9]
#define  T_SPACE      	CInput::getInstance()->key[KEY_SPACE]


#define  TR_UP          CInput::getInstance()->keyR[KEY_UP]
#define  TR_LEFT        CInput::getInstance()->keyR[KEY_LEFT]
#define  TR_RIGHT       CInput::getInstance()->keyR[KEY_RIGHT]
#define  TR_DOWN        CInput::getInstance()->keyR[KEY_DOWN]

typedef struct{
	Uint32 codeKey;
}tEventKey,tEventRepeatKey;


class CInput{
	static CInput *input;
	SDL_Event Event;
	char *buffer;


public:
	bool            key[KEY_LAST];
	bool            keyR[KEY_LAST];

	static CInput * getInstance();
	static void destroy();

	CInput();

	//void setBufferKeyInput(char *buffer);

	SDL_Event * update();

};
