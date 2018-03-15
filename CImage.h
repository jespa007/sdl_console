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

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK32  0xff000000
#define GMASK32  0x00ff0000
#define BMASK32  0x0000ff00
#define AMASK32  0x000000ff
#else
#define RMASK32  0x000000ff
#define GMASK32  0x0000ff00
#define BMASK32  0x00ff0000
#define AMASK32  0xff000000
#endif

class CImage{

protected:
	SDL_Texture *texture;
	void destroy();
	int mWidth,mHeight;

	static SDL_Texture * SurfaceToTexture(SDL_Surface *srf);
	static SDL_Surface * createSurface(int width, int height);
public:
	CImage();



	bool load(const char *file);

	SDL_Texture *getTexture();


	int getWidth();
	int getHeight();

	~CImage();
};
