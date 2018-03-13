/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

#include   <SDL2/SDL.h>

class CImage{

protected:
	SDL_Texture *texture;
	void destroy();
	int mWidth,mHeight;

	static SDL_Texture * SurfaceToTexture(SDL_Surface *srf);

public:
	CImage();

	bool load(const char *file);

	SDL_Texture *getTexture();


	int getWidth();
	int getHeight();

	~CImage();
};
