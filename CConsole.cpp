#include "CConsole.h"
#include "font_8x16.c"
#include "font_8x16s.c"
#include "font_8x8.c"

#include <iostream>

#define MOUSE_QUAD_COLLIDE(x1,y1,x2,y2) ((x1)<=(xmouse) && xmouse <= (x2) && (y1)<=ymouse && ymouse <= (y2))
#define SHIFT_OR_CAPS_ON(e) (e.key.keysym.mod & (KMOD_SHIFT|KMOD_CAPS))

#define MOUSE_COLLIDE_COPY_ITEM  (MOUSE_QUAD_COLLIDE(xpopup,ypopup,xpopup+POPUP_WIDTH,ypopup+16))
#define MOUSE_COLLIDE_PASTE_ITEM (MOUSE_QUAD_COLLIDE(xpopup,ypopup+16+4,xpopup+POPUP_WIDTH,ypopup+32+4))

#define N_LINES_TEXT_WRAP(text) ((text.size()/CHARS_PER_WIDTH)+1)

#define ALERT_WIDTH  80
#define ALERT_HEIGHT 40

#define POPUP_WIDTH  50
#define POPUP_HEIGHT 50

#define MIN(a,b) ((a)<(b)?(a):(b))

#define CHAR_WIDTH  8
#define CHAR_HEIGHT 16

CConsole *CConsole::console=NULL;

CConsole::CConsole(){

	input = CInput::getInstance();
	console_font = NULL;

	start_ms=0;
	is_blink=false;


	CONSOLE_WIDTH = 0;
	CONSOLE_HEIGHT = 0;

	CHARS_PER_WIDTH=0;
	CHARS_PER_HEIGHT=0;

	pWindow = NULL;
	pRenderer=NULL;
	fullscreen=false;

	currentX=0;
	currentY=0;

	char_cursor=0;
	history_cursor=-1;

	prompt=">";

	selecting=false;
	start_select_char=-1;
	end_select_char=-1;
	x1_sel=-1;
	x2_sel=-1;
	y2_sel=-1;
	y1_sel=-1;
	popup_font=NULL;
	edit_copy_popup_open=false;
	quit=false;
	xmouse=0;
	ymouse=0;
	xpopup=0;
	ypopup=0;

	alert_timeout=0;
	text_alert="";
}

bool CConsole::blink_500ms(){
	if(start_ms<SDL_GetTicks()){
		start_ms=SDL_GetTicks()+500;
		is_blink=!is_blink;
	}
	return is_blink;
}

void CConsole::setPrompt(const char *_prompt){
	prompt=_prompt;
	output=prompt;
	char_cursor=output.size();
}

void CConsole::setOutput(const string & _output){
	output=prompt+_output;
	char_cursor=output.size();
}


void CConsole::init(int col_width, int col_height){

	if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO) {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr,"Unable to init video subsystem: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	//setFont(font);
	CONSOLE_WIDTH=col_width*CHAR_WIDTH;//font->getCharWidth();
	CONSOLE_HEIGHT=col_height*CHAR_HEIGHT;//font->getCharHeight();
	CHARS_PER_WIDTH=col_width;
	CHARS_PER_HEIGHT=col_height;

	pWindow = SDL_CreateWindow(
			"SDL console"
		    ,SDL_WINDOWPOS_UNDEFINED
			,SDL_WINDOWPOS_UNDEFINED
			,CONSOLE_WIDTH
			,CONSOLE_HEIGHT
			,0);

	if (!pWindow) {
		fprintf(stderr,"Unable to create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_WINDOW_SHOWN);

    if (!pRenderer) {
		fprintf(stderr,"Unable to create renderer: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
    }

	console_font = new CFont();
	//font->load("font.bmp",8,16);
	console_font->load_bitmapped((const unsigned char *)font_8x16,CHAR_HEIGHT,256);

	popup_font = new CFont();
	popup_font->load_bitmapped((const unsigned char *)font_8x16s,16,256);


    //Enable text input
    SDL_StartTextInput();
}

void CConsole::setFont(CFont * _font){
/*	font = _font;
	CHARS_PER_WIDTH=CONSOLE_WIDTH/font->getCharWidth();
	CHARS_PER_HEIGHT=CONSOLE_HEIGHT/font->getCharHeight();

	printf("Set console %ix%i\n",CHARS_PER_WIDTH,CHARS_PER_HEIGHT);*/
}

CConsole *CConsole::getInstance(){
	if(console == NULL){
		console = new CConsole();
	}
	return console;
}

void CConsole::destroy(){
	if(console){
		delete console;
	}
	console = NULL;
}

int CConsole::getWidth(){
	return CONSOLE_WIDTH;
}

int CConsole::getHeight(){
	return CONSOLE_HEIGHT;
}


SDL_Renderer * CConsole::getRenderer(){
	return pRenderer;
}

void CConsole::clear(Uint8 r, Uint8 g, Uint8 b){
	//Clear screen
	SDL_SetRenderDrawColor( pRenderer, r, g, b, 0xFF );
	SDL_RenderClear( pRenderer );
}


SDL_Rect *CConsole::getCurrentCursor(int x,int y, const char * c_text){
	if(console_font){
		SDL_Texture *font_text=console_font->getTexture();
		if(font_text){

			rect_textout={x,y,console_font->getCharWidth(),console_font->getCharHeight()};
			for(int i=0; i < MIN(char_cursor,(int)strlen(c_text)); i++){


				if((rect_textout.x+console_font->getCharWidth())>CONSOLE_WIDTH){ // carry return ...
					rect_textout.y+=rect_textout.h;
					rect_textout.x=0;
				}
				rect_textout.x+=rect_textout.w;
			}

			// correct offset as needed...
			if((rect_textout.x+console_font->getCharWidth())>CONSOLE_WIDTH){ // carry return ...
				rect_textout.y+=rect_textout.h;
				rect_textout.x=0;
			}

			return &rect_textout;
		}
	}

	return NULL;
}

SDL_Rect *CConsole::drawText(int x,int y, const char * c_text, CFont *font, int rgb, unsigned int properties){
	if(font){
		SDL_Texture *font_text=font->getTexture();
		if(font_text){

			if(properties & CENTER_TEXT_PROPERTY){ // only works with texts without carry returns...
				x-=(font->getCharWidth()*strlen(c_text))/2;
			}


			rect_textout={x,y,font->getCharWidth(),font->getCharHeight()};
			for(unsigned i=0; i < strlen(c_text); i++){
				char c=c_text[i];



				if(((rect_textout.x+font->getCharWidth())>CONSOLE_WIDTH) && (properties & WRAP_WINDOW_PROPERTY)){ // carry return ...
					rect_textout.y+=rect_textout.h;
					rect_textout.x=0;
				}


				SDL_SetTextureColorMod(font_text,
										 rgb&0xFF,
									    (rgb>>8)&0xFF,
									    (rgb>>16)&0xFF);


				//else{ // advance ...
				if(start_select_char!=-1){

					if(
							x1_sel <= rect_textout.x && rect_textout.x <= x2_sel &&
							y1_sel <= rect_textout.y && rect_textout.y <= y2_sel
					){
					SDL_SetTextureColorMod(font_text,
							 0,
						    0x1F,
						    0);

					}
				}


				SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c), &rect_textout);
				rect_textout.x+=rect_textout.w;
				//}
			}

			// correct offset as needed...
			if(((rect_textout.x+font->getCharWidth())>CONSOLE_WIDTH) && (properties & WRAP_WINDOW_PROPERTY)){ // carry return ...
				rect_textout.y+=rect_textout.h;
				rect_textout.x=0;
			}

			return &rect_textout;
		}
	}

	return NULL;
}

void CConsole::drawChar(int x,int y, char c_text, CFont *font,int rgb){
	if(font){
		SDL_Texture *font_text=font->getTexture();
		if(font_text){
			SDL_SetTextureColorMod(font_text,
					 rgb&0xFF,
				   (rgb>>8)&0xFF,
				   (rgb>>16)&0xFF);

			SDL_Rect rect={x,y,font->getCharWidth(),font->getCharHeight()};
			SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c_text), &rect);
		}
	}
}

void CConsole::toggleFullscreen(){
	if(!fullscreen){
		SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN);
	}else{
		SDL_SetWindowFullscreen(pWindow, 0);
	}

	fullscreen=!fullscreen;
}


CConsole::tConsoleLineOutput * CConsole::print(const char *c){
	string str=c;
	tConsoleLineOutput clo;
	clo.n_lines=N_LINES_TEXT_WRAP(str);

	clo.text=(char **)malloc(sizeof(char *)*clo.n_lines);
	unsigned total_length=strlen(c);

	for(int i=0; i < clo.n_lines; i++){
		unsigned len =CHARS_PER_WIDTH;
		if(total_length<(unsigned)CHARS_PER_WIDTH){
			len=total_length;
		}

		clo.text[i]=(char *)malloc(sizeof(char)*(len+1));
		memset(clo.text[i],0,sizeof(char)*(len+1));
		strncpy(clo.text[i],c,len+1);

		total_length-=len;
		c+=len;
	}

	console_line_output.push_back(clo);
	return &console_line_output[console_line_output.size()-1];
}

std::vector<std::string> split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}


void CConsole::printError(const char *text_in,...){
	char text_out[4096];

	va_list  ap;
	va_start(ap,  text_in);
	vsprintf(text_out,  text_in,  ap);
	va_end(ap);

	vector<string> line=split_string(text_out,"\n");

	for(unsigned l=0; l < line.size(); l++){
		tConsoleLineOutput * clo = print(line[l].c_str());
		clo->rgb=RMASK32;
	}
}

void CConsole::printOut(const char *text_in,...){
	char text_out[4096];
	va_list  ap;
	va_start(ap,  text_in);
	vsprintf(text_out,  text_in,  ap);
	va_end(ap);

	vector<string> line=split_string(text_out,"\n");

	for(unsigned l=0; l < line.size(); l++){
		print(line[l].c_str());
	}
}

int CConsole::getOffsetConsolePrint(int & intermid_line){

	//int n_lines = 0;
	intermid_line=0;
	int offset = console_line_output.size()-1;
	int n_lines_output=N_LINES_TEXT_WRAP(output);
	int n_lines=0;

	//if(offset>offset_output)


	if(offset>=0){

		do{
			n_lines+=console_line_output[offset--].n_lines;

		}while(offset>=0 && ((n_lines+n_lines_output) < CHARS_PER_HEIGHT));


		// correct offset...
		offset++;


		if((n_lines+n_lines_output)>=CHARS_PER_HEIGHT){
			intermid_line=(n_lines+n_lines_output)-CHARS_PER_HEIGHT;//-(N_LINES_TEXT_WRAP(output)-1);
			if(intermid_line==0){ // no lines but
				//offset++;
			}
		}


		/*if(n_lines>=(CHARS_PER_HEIGHT-1)){
			// rest output...
			offset+=n_lines_output;
		}*/
	}


	return offset;
}

unsigned CConsole::getTotalLines(){
	unsigned n_lines =0;
	for(unsigned i = 0; i < console_line_output.size(); i++){
		n_lines+=console_line_output[i].n_lines;
	}
	return n_lines+N_LINES_TEXT_WRAP(output);
}

void CConsole::toggleApplicationPopup(){
	edit_copy_popup_open=!edit_copy_popup_open;
	if(edit_copy_popup_open){
		xpopup=xmouse;
		ypopup=ymouse;
	}
}

void CConsole::updateApplicationPopup(){
	if(edit_copy_popup_open){



		/*SDL_Rect fillRect = { xpopup, ypopup, POPUP_WIDTH,POPUP_HEIGHT  };
		SDL_SetRenderDrawColor( pRenderer, 0x1F, 0x1F, 0x1F, 0xFF );
		SDL_RenderFillRect( pRenderer, &fillRect );
		fillRect = { xpopup, ypopup, POPUP_WIDTH-1,POPUP_HEIGHT-1  };
		SDL_SetRenderDrawColor( pRenderer, 0x7F, 0x7F, 0x7F, 0xFF );

		SDL_RenderFillRect( pRenderer, &fillRect );*/

		renderApplicationPopup();
	}


}

void CConsole::renderApplicationPopup(){
	if(edit_copy_popup_open){

		int x=xpopup,  y=ypopup;

		// background ...
		SDL_Rect fillRect = { x, y, POPUP_WIDTH,POPUP_HEIGHT  };
		SDL_SetRenderDrawColor( pRenderer, 0x1F, 0x1F, 0x1F, 0xFF );
		SDL_RenderFillRect( pRenderer, &fillRect );
		fillRect = { x, y, POPUP_WIDTH-2,POPUP_HEIGHT-2  };
		SDL_SetRenderDrawColor( pRenderer, 0x7F, 0x7F, 0x7F, 0xFF );

		SDL_RenderFillRect( pRenderer, &fillRect );

		// Render options...

		y+=5;
		drawText(x+(POPUP_WIDTH>>1),y,"Copy",popup_font,(start_select_char < end_select_char)?(MOUSE_COLLIDE_COPY_ITEM?0x00FF00:0xFFFFFF):0x1F1F1F,CENTER_TEXT_PROPERTY);
		drawText(x+(POPUP_WIDTH>>1),y+=(16+4),"Paste",popup_font,MOUSE_COLLIDE_PASTE_ITEM?0x00FF00:0xFFFFFF,CENTER_TEXT_PROPERTY);


	}
}

void CConsole::alert(int time,const char *s,...){
	alert_timeout = SDL_GetTicks()+time;

	text_alert=s;
}

void CConsole::updateAlert(){
	if(SDL_GetTicks() < alert_timeout){
		renderAlert();
	}
}

void CConsole::renderAlert(){
	SDL_Rect fillRect = { (CONSOLE_WIDTH>>1)-(ALERT_WIDTH>>1), (CONSOLE_HEIGHT>>1)-(ALERT_HEIGHT>>1), ALERT_WIDTH,ALERT_HEIGHT  };
	SDL_SetRenderDrawColor( pRenderer, 0x1F, 0x1F, 0x1F, 0xFF );
	SDL_RenderFillRect( pRenderer, &fillRect );

	drawText((CONSOLE_WIDTH>>1),(CONSOLE_HEIGHT>>1),text_alert,popup_font,-1,CENTER_TEXT_PROPERTY);
}


const char * CConsole::update(){
	bool cr=false;
	bool typed_input=false;

	// clear screen...
	clear(0,0,0);



	// draw selected char...
	if((start_select_char<end_select_char)){

		// draw background char by char
		int p = start_select_char;
		int w=console_font->getCharWidth();
		int h=console_font->getCharHeight();


		SDL_Rect fillRect = { x1_sel, y1_sel, console_font->getCharWidth(), console_font->getCharHeight() };
		SDL_SetRenderDrawColor( pRenderer, 0x1F, 0x1F, 0x1F, 0x1F );
		do{


			SDL_RenderFillRect( pRenderer, &fillRect );

			if((fillRect.x + w) >= CONSOLE_WIDTH){
				fillRect.x=0;
				fillRect.y+=h;
			}
			else{
				fillRect.x+=w;
			}
			p++;

		}while(p<end_select_char);


		//printf("%i %i %i %i\n",x1, y1, x2-x1, y2-y1);



	}


	// update keyboard events...
	int intermid=0;
	int offset=getOffsetConsolePrint(intermid);
	int offsetY=0;
	SDL_Rect * rect=0;
	int lines=0;
	const char *output_start=output.c_str();
	string console_text="";

	for(unsigned i=offset; i < console_line_output.size(); i++){
		for(int l=((int)i==offset)?intermid:0; l < console_line_output[i].n_lines; l++){
			rect=drawText(0,offsetY,console_line_output[i].text[l],console_font,console_line_output[i].rgb);
			console_text+=console_line_output[i].text[l]+string("\n");
			offsetY+=rect->h;
			lines++;
		}
	}

	int starting_line=N_LINES_TEXT_WRAP(output);
	if((starting_line)>(CHARS_PER_HEIGHT)){
		starting_line-=(CHARS_PER_HEIGHT);
		output_start=output.c_str()+CHARS_PER_WIDTH*starting_line;
	}

	if(cr){

		print(output.c_str());
		str_type_input=(char *)(output.c_str()+prompt.size());
		cr=false;
		typed_input = true;

		history.push_back(str_type_input);
		history_cursor=history.size();
		setOutput("");

	}

	console_text+=string(output_start);
	drawText(0,offsetY,output_start,console_font);

	if(blink_500ms()){

		rect = getCurrentCursor(0,offsetY,output_start);
		if(rect){
			drawChar(rect->x,rect->y,22,console_font);
		}
	}

	while( SDL_PollEvent( &e ) )
	{
		switch(e.type) {
			case SDL_MOUSEBUTTONDOWN:

				xmouse=e.button.x;
				ymouse=e.button.y;

				if(e.button.button==SDL_BUTTON_LEFT){


					if(edit_copy_popup_open){

						if(start_select_char < end_select_char){
							if(MOUSE_COLLIDE_COPY_ITEM){ // copy selected text
								alert(1000,"Text copied");
							}
						}

						edit_copy_popup_open=false;
					}



						start_select_char=(e.button.x/CHAR_WIDTH+((e.button.y/CHAR_HEIGHT)*CHARS_PER_WIDTH));
						end_select_char=(e.button.x/CHAR_WIDTH+((e.button.y/CHAR_HEIGHT)*CHARS_PER_WIDTH));

						x1_sel=(start_select_char%CHARS_PER_WIDTH)*(CHAR_WIDTH);
						y1_sel=(start_select_char/CHARS_PER_WIDTH)*CHAR_HEIGHT;//*CONSOLE_WIDTH;

						x2_sel=(end_select_char%CHARS_PER_WIDTH)*(CHAR_WIDTH);
						y2_sel=(end_select_char/CHARS_PER_WIDTH)*CHAR_HEIGHT;//*CONSOLE_WIDTH;

						selecting=true;

				}

				if(e.button.button==SDL_BUTTON_RIGHT){
					toggleApplicationPopup();
				}
				break;

			case SDL_MOUSEBUTTONUP:
				xmouse=e.button.x;
				ymouse=e.button.y;


				if(e.button.button==SDL_BUTTON_LEFT){
					selecting=false;
				}
				break;
			case SDL_MOUSEMOTION:
				xmouse=e.button.x;
				ymouse=e.button.y;
				if(selecting){


					end_select_char=(e.button.x/CHAR_WIDTH+((e.button.y/CHAR_HEIGHT)*CHARS_PER_WIDTH));
					x2_sel=(end_select_char%CHARS_PER_WIDTH)*(CHAR_WIDTH);
					y2_sel=(end_select_char/CHARS_PER_WIDTH)*CHAR_HEIGHT;//*CONSOLE_WIDTH;

				}
				break;
			case SDL_KEYDOWN:

				switch(e.key.keysym.sym){
					case 	KEY_v:
						if(SDL_GetModState() & KMOD_CTRL){
							output += SDL_GetClipboardText();
						}
						break;
					case 	KEY_c:
						if(SDL_GetModState() & KMOD_CTRL){

							if(start_select_char<end_select_char){
								string copy=console_text.substr(start_select_char,end_select_char);

								printf("copy %i %i %s\n",start_select_char,end_select_char,copy.c_str());
								start_select_char=-1;
								end_select_char=-1;
							}
							//SDL_GetClipboardText();
						}
						break;
					case SDLK_RETURN:
						cr=true;
						break;
					case SDLK_UP: // history up...
						if(history_cursor>0){
							setOutput(history[--history_cursor]);
						}
						break;
					case SDLK_DOWN: // history down...
						if((unsigned)history_cursor<history.size()){
							setOutput(history[history_cursor++]);
						}
						break;
					case SDLK_LEFT: // cursor left...
						if((unsigned)char_cursor>prompt.size()){
							char_cursor--;
						}
						break;
					case SDLK_RIGHT: // cursor right...
						if((unsigned)char_cursor<output.size()){
							char_cursor++;
						}
						break;
					case SDLK_ESCAPE:
						quit=true;
						break;
				//	case SDLK_F5: printError("hello"); break;
				//	case SDLK_F9: toggleFullscreen(); break;
					case SDLK_BACKSPACE:
						if((output.size()-prompt.size()) > 0){
							output.pop_back();
						}
						break;
					case SDLK_APPLICATION:
						toggleApplicationPopup();
						//printf("key 0x%X\n",e.key.keysym.sym);
						break;
					default:
						break;
				}
				break;

			case SDL_TEXTINPUT:
				//Not copy or pasting
				if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && (SDL_GetModState() & KMOD_CTRL) ) )
				{
					output.insert(MIN((int)output.size(),char_cursor),string(e.text.text));
					char_cursor+=strlen(e.text.text);
				}
				break;
		}

	}

	updateApplicationPopup();
	updateAlert();



	// update screen...
	SDL_RenderPresent( pRenderer );

	if(typed_input){
		return str_type_input.c_str();
	}

	return NULL;
}


bool CConsole::end(){
	return quit;
}

CConsole::~CConsole(){
	if(pRenderer != NULL){
		SDL_DestroyRenderer(pRenderer);
	}

	if(pWindow != NULL){
		SDL_DestroyWindow(pWindow);
	}

	pRenderer=NULL;
	pWindow=NULL;

	 //Disable text input
	 SDL_StopTextInput();

	SDL_Quit();
}
