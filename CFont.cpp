/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "CFont.h"
#include "CConsole.h"



CFont::CFont(){

	char_width=char_height=totalchars=totalchars_x=totalchars_y=0;
}

bool CFont::load(SDL_Surface * srf,int char_width,int char_height){

	destroy();

	if((texture=CImage::SurfaceToTexture(srf)) !=NULL){
		/*SDL_QueryTexture(texture,
				NULL,
				NULL,
				&this->mWidth,
				&this->mHeight);*/
		this->mWidth=srf->w;
		this->mHeight=srf->h;
		this->char_width=char_width;
		this->char_height=char_height;
		totalchars_x=this->mWidth/char_width;
		totalchars_y=this->mHeight/char_height;
		totalchars=totalchars_x*totalchars_y;

		printf("Loaded font (%ix%i). Font size: %ix%i. wc:%i hc:%i total chars:%i\n",this->mWidth,this->mHeight,char_width,char_height,totalchars_x,totalchars_y,totalchars);

		return true;
	}
	else{
		fprintf(stderr,"SurfaceToTexture:%s\n",SDL_GetError());
	}

	return false;
}

bool CFont::load(const char * file,int char_width,int char_height){

	SDL_Surface *srf=SDL_LoadBMP(file);
	if(srf!=NULL){

		return load(srf,char_width,char_height);
	}
	else{
		fprintf(stderr,"SDL_LoadBMP:%s\n",SDL_GetError());
	}


	return false;
}

bool CFont::load_bitmapped(const unsigned char *pixelmap,int _char_height, int total_chars){

	unsigned _char_width=8;
	unsigned scanline_width=_char_width*total_chars*4;

	SDL_Surface *srf=createSurface(_char_width*total_chars,_char_height);
	if(srf!=NULL){

		unsigned char *pixels=NULL;//(unsigned int *)srf->pixels;
		unsigned int offsetX=0;

		//char charmap[13]={0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18};

		for(int c=0; c < total_chars;c++){

			for(int y=0; y < _char_height ;y++){
				pixels=((unsigned char *)srf->pixels + (y)*scanline_width+offsetX);
				for(unsigned x=0;x<_char_width;x++){
					unsigned int pm=*((pixelmap + c * _char_height) + y);// = j + 2;
					//unsigned int pm=(const unsigned char **)[c][y];
					if(pm & (0x1<<x)){
						*((unsigned int *)pixels)=RMASK32|GMASK32|BMASK32;
					}
					pixels+=4;
				}
			}

			offsetX+=(_char_width*4);
		}

		return load(srf,_char_width,_char_height);
	}
	else{
		fprintf(stderr,"SDL_LoadBMP:%s\n",SDL_GetError());
	}


	return false;

}

int CFont::getCharWidth(){
	return char_width;
}
int CFont::getCharHeight(){
	return char_height;
}

int CFont::getTextWith(const std::string & str){

	int total_width=0;

	for(unsigned i=0; i < str.size(); i++){
		total_width+=char_width;
	}
	return total_width;
}

SDL_Rect * CFont::getRectChar(char c){
	m_aux_rect={0,0,char_width,char_height};
	if(c<totalchars){
		int x=c%totalchars_x;
		int y=c/totalchars_x;


		m_aux_rect.x=x*char_width;
		m_aux_rect.y=y*char_height;

	}


	return &m_aux_rect;
}

CFont::~CFont(){

}
