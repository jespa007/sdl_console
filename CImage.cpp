/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "CConsole.h"



SDL_Texture *  CImage::SurfaceToTexture(SDL_Surface *srf){

	SDL_Texture *text=SDL_CreateTextureFromSurface( CConsole::getInstance()->getRenderer(), srf );

	if(!text){
		fprintf(stderr,"Error converting surface %s\n",SDL_GetError());
	}
	return text;
}



SDL_Surface *  CImage::createSurface(int width, int height){

	 SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 32,
	                                   RMASK32, GMASK32, BMASK32, AMASK32);
	    if (surface == NULL) {
	    	fprintf(stderr,"SDL_CreateRGBSurface() failed: %s", SDL_GetError());

	    }
	return surface;
}

CImage::CImage(){
	mWidth=	mHeight=0;
	texture=NULL;
	// default pixmap..
}



// create image from native ...
bool CImage::load(const char *file){
	SDL_Surface *srf=SDL_LoadBMP(file);
	if(srf!=NULL){
		if((texture=CImage::SurfaceToTexture(srf)) !=NULL){
			this->mWidth=srf->w;
			this->mHeight=srf->h;

			printf("Loaded image %s (%ix%i).\n",file,this->mWidth,this->mHeight);

			return true;
		}
		else{
			fprintf(stderr,"SurfaceToTexture:%s\n",SDL_GetError());
		}
	}
	else{
		fprintf(stderr,"SDL_LoadBMP:%s\n",SDL_GetError());
	}


	return false;
}

SDL_Texture * CImage::getTexture(){
	return texture;
}

int CImage::getWidth(){
	return mWidth;
}

int CImage::getHeight(){
	return mHeight;
}

void CImage::destroy(){

	if(texture != NULL){
		printf("Image destroyed!\n");
		SDL_DestroyTexture(texture);
	}
	texture=NULL;
}

CImage::~CImage(){
	destroy();
}
