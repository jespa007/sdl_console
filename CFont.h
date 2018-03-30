/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "CImage.h"


#define SDL_CONSOLE_MAJOR 0
#define SDL_CONSOLE_MINOR 0
#define SDL_CONSOLE_PATCH 1


class CFont:public CImage{

	int totalchars_x, totalchars_y,totalchars;
	SDL_Rect m_aux_rect;
	int char_width,char_height;

	bool load(SDL_Surface * srf,int char_width,int char_height);
public:

	CFont();

	bool load(const char * file,int char_width,int char_height);
	bool load_bitmapped(const unsigned char *pixelmap,int char_height, int total_chars);

	int getCharWidth();
	int getCharHeight();
	int getTextWith(const std::string & str);
	SDL_Rect * getRectChar(char c);

	~CFont();

};
