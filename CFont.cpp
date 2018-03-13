/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "CFont.h"

CFont::CFont(){

	char_width=char_height=totalchars=totalchars_x=totalchars_y=0;
}

bool CFont::load(const char * file,int char_width,int char_height){

	SDL_Surface *srf=SDL_LoadBMP(file);
	if(srf!=NULL){
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

			printf("Loaded font %s (%ix%i). Font size: %ix%i. wc:%i hc:%i\n",file,this->mWidth,this->mHeight,char_width,char_height,totalchars_x,totalchars_y);

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
