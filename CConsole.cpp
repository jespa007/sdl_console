#include "CConsole.h"

#define RMASK32 0x000000ff
#define GMASK32 0x0000ff00
#define BMASK32 0x00ff0000
#define AMASK32 0xff000000
#define SHIFT_OR_CAPS_ON(e) (e.key.keysym.mod & (KMOD_SHIFT|KMOD_CAPS))

#define N_LINES_TEXT_WRAP(text) ((text.size()/CHARS_PER_WIDTH)+1)

CConsole *CConsole::console=NULL;

CConsole::CConsole(){

	input = CInput::getInstance();
	font = NULL;

	start_ms=0;
	is_blink=false;

	CHARS_PER_WIDTH=

	CONSOLE_WIDTH = 0;
	CONSOLE_HEIGHT = 0;

	CHARS_PER_WIDTH=0;
	CHARS_PER_HEIGHT=0;

	pWindow = NULL;
	pRenderer=NULL;
	fullscreen=false;

	currentX=0;
	currentY=0;

	total_lines=0;

}

bool CConsole::blink_500ms(){
	if(start_ms<SDL_GetTicks()){
		start_ms=SDL_GetTicks()+500;
		is_blink=!is_blink;
	}
	return is_blink;
}

void CConsole::init(int width, int height){

	if (SDL_WasInit(SDL_INIT_VIDEO) != SDL_INIT_VIDEO) {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr,"Unable to init video subsystem: %s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	pWindow = SDL_CreateWindow(
			"SDL console"
		    ,SDL_WINDOWPOS_UNDEFINED
			,SDL_WINDOWPOS_UNDEFINED
			,CONSOLE_WIDTH=width
			,CONSOLE_HEIGHT=height
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

    //Enable text input
    SDL_StartTextInput();
}

void CConsole::setFont(CFont * _font){
	font = _font;
	CHARS_PER_WIDTH=CONSOLE_WIDTH/font->getCharWidth();
	CHARS_PER_HEIGHT=CONSOLE_HEIGHT/font->getCharHeight();

	printf("Set console %ix%i\n",CHARS_PER_WIDTH,CHARS_PER_HEIGHT);
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


SDL_Rect *CConsole::drawText(int x,int y, const char * c_text, int rgb){
	if(font){
		SDL_Texture *font_text=font->getTexture();
		if(font_text){

			SDL_SetTextureColorMod(font_text,
					 rgb&0xFF,
				    (rgb>>8)&0xFF,
				    (rgb>>16)&0xFF);

			rect_textout={x,y,font->getCharWidth(),font->getCharHeight()};
			for(unsigned i=0; i < strlen(c_text); i++){
				char c=c_text[i];


				if((rect_textout.x+font->getCharWidth())>CONSOLE_WIDTH){ // carry return ...
					rect_textout.y+=rect_textout.h;
					rect_textout.x=0;
				}
				//else{ // advance ...
				SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c), &rect_textout);
				rect_textout.x+=rect_textout.w;
				//}
			}

			// correct offset as needed...
			if((rect_textout.x+font->getCharWidth())>CONSOLE_WIDTH){ // carry return ...
				rect_textout.y+=rect_textout.h;
				rect_textout.x=0;
			}

			return &rect_textout;
		}
	}

	return NULL;
}

void CConsole::drawChar(int x,int y, char c_text, int rgb){
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

void CConsole::printError(const char *c){
	tConsoleLineOutput * clo = print(c);
	clo->rgb=RMASK32;
}

void CConsole::printOut(const char *c){
	tConsoleLineOutput * clo = print(c);
}

int CConsole::getOffsetConsolePrint(int & intermid_line){

	int n_lines = 0;
	intermid_line=0;
	int offset = console_line_output.size()-1;
	int offset_output=N_LINES_TEXT_WRAP(output);

	if(offset>offset_output)


	if(offset>0){

		do{
			n_lines+=console_line_output[offset--].n_lines;

		}while(offset>0 && (n_lines < CHARS_PER_HEIGHT));

		if(n_lines>=CHARS_PER_HEIGHT){
			intermid_line=n_lines-CHARS_PER_HEIGHT+1;

		}

		if(n_lines>=(CHARS_PER_HEIGHT-1)){
			// rest output...
			offset+=N_LINES_TEXT_WRAP(output);
		}



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

bool CConsole::update(){
	// clear screen...
	clear(0,0,0);

	// update keyboard events...
	bool cr=false;

	int intermid=0;
	int offset=getOffsetConsolePrint(intermid);
	int offsetY=0;
	SDL_Rect * rect=0;

	for(unsigned i=offset; i < console_line_output.size(); i++){
		for(int l=((int)i==offset)?intermid:0; l < console_line_output[i].n_lines; l++){
			rect=drawText(0,offsetY,console_line_output[i].text[l],console_line_output[i].rgb);
			offsetY+=rect->h;
		}
	}

	while( SDL_PollEvent( &e ) )
	{
		switch(e.type) {
			case SDL_KEYDOWN:

				switch(e.key.keysym.sym){
					case 	KEY_v:
						if(SDL_GetModState() & KMOD_CTRL){
							output += SDL_GetClipboardText();
						}
						break;
					case SDLK_RETURN:
						cr=true;
						break;
					case SDLK_UP:  break;
					case SDLK_DOWN:  break;
					case SDLK_LEFT:   break;
					case SDLK_RIGHT: break;
					case SDLK_ESCAPE:
						return false;
						break;
					case SDLK_F5: printError("hello"); break;
					case SDLK_F9: toggleFullscreen(); break;
					case SDLK_BACKSPACE:
						if(output.length() > 0){
							output.pop_back();
						}
						break;
					default:
						break;
				}
				break;

			case SDL_TEXTINPUT:
				//Not copy or pasting
				if( !( ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' ) && ( e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) && (SDL_GetModState() & KMOD_CTRL) ) )
				{
					output += e.text.text;
				}
				break;
		}

	}

	if(cr){
		tConsoleLineOutput * clo=print(output.c_str());
		//total_lines+=clo.n_lines;
		output="";
		cr=false;
	}

	rect = drawText(0,offsetY,output.c_str());

	if(blink_500ms()){
		drawChar(rect->x,rect->y,22);
	}

	// update screen...
	SDL_RenderPresent( pRenderer );

	return true;
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
