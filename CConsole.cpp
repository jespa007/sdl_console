#include "CConsole.h"
#include "font_8x16.c"
#include "font_8x16s.c"
#include "font_8x8.c"

#include <iostream>

#define MOUSE_QUAD_COLLIDE(x1,y1,x2,y2) ((x1)<=(xmouse) && xmouse <= (x2) && (y1)<=ymouse && ymouse <= (y2))
#define SHIFT_OR_CAPS_ON(e) (e.key.keysym.mod & (KMOD_SHIFT|KMOD_CAPS))

#define MOUSE_COLLIDE_COPY_ITEM  (MOUSE_QUAD_COLLIDE(xpopup,ypopup,xpopup+POPUP_WIDTH,ypopup+16))
#define MOUSE_COLLIDE_PASTE_ITEM (MOUSE_QUAD_COLLIDE(xpopup,ypopup+16+4,xpopup+POPUP_WIDTH,ypopup+32+4))

#define ALERT_WIDTH  160
#define ALERT_HEIGHT 40

#define POPUP_WIDTH  50
#define POPUP_HEIGHT 50

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define CHAR_WIDTH  8
#define CHAR_HEIGHT 16

#define TAB_CHAR_WIDTH 4

CConsole *CConsole::console=NULL;
const char * CConsole::sdl_console_caption="SDL console";

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

	select_start_line=-1;
	select_start_col=-1;

	selecting=false;
	line_ini=-1;
	col_ini=-1;
	line_end=-1;
	col_end=-1;

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

void CConsole::setCaption(const char *caption_name){
	sdl_console_caption=caption_name;
}

int CConsole::N_LINES_TEXT_WRAP(const string & c_text){

	int n_lines=1;
	int n_chars=0;

	for(unsigned i=0; i < c_text.size(); i++){

		if((n_chars)>=(CHARS_PER_WIDTH-1) || c_text[i]=='\n'){ // carry return ...
			rect_textout.y+=rect_textout.h;
			n_chars=0;
			n_lines++;
		}
		else{
			n_chars++;
		}
	}
	return n_lines;
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
			sdl_console_caption
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
			int current_char=0;//prompt.size();
			int max_cursor=MIN(char_cursor,(int)strlen(c_text));

			char last_char=0;

			while(current_char < max_cursor){

				last_char=c_text[current_char];

				//else
				{
					if(rect_textout.x>=CONSOLE_WIDTH ){ // carry return ...
						rect_textout.y+=rect_textout.h;
						rect_textout.x=0;
					}else{

					}

					if(last_char=='\n'){
							rect_textout.y+=rect_textout.h;
							rect_textout.x=0;
							//current_char++;
					}
					else{
						rect_textout.x+=rect_textout.w;
					}
				}
				current_char++;
			}

			// at the end check
			{

				if(rect_textout.x>=CONSOLE_WIDTH ){ // carry return ...
					rect_textout.y+=rect_textout.h;
					rect_textout.x=0;

					if((unsigned)current_char<strlen(c_text)){
									if(c_text[current_char]=='\n'){
										rect_textout.y+=rect_textout.h;
										rect_textout.x=0;
									}
								}

				}
			}

			return &rect_textout;
		}
	}
	return NULL;
}

CConsole::tConsoleLineOutput * CConsole::print(const char *to_print_str){
	vector<string> vec_str;//=c;
	string str_line;
	tConsoleLineOutput clo;
	clo.n_lines=0;
	int n_chars_line=0;

	str_line="";
	char *ptr=(char *)to_print_str;
	char buffer[4096];
	sprintf(buffer,"%s\n",to_print_str);
	ptr=buffer;

	// scan lines ++
	while(*ptr){

		int repeat_char =1;
		char c=(*ptr);

		switch(*ptr){

		case '\n':
			repeat_char =0;
			n_chars_line=0;
			str_line+="\n"; // the carry return should be take account in order to have a right copy later...
			vec_str.push_back(str_line);
			str_line="";
			break;
		case '\r': // ignore ...
			repeat_char =0;
			break;
		/*case '\t':
			c=' ';
			repeat_char=TAB_CHAR_WIDTH;
			break;*/
		default:
			break;
		}

		for(int r=0; r < repeat_char; r++){
			str_line+=c;
			n_chars_line++;

			if(n_chars_line>=CHARS_PER_WIDTH){
				n_chars_line=0;
				vec_str.push_back(str_line);
				str_line="";
			}
		}
		ptr++;
	}

	if(str_line != ""){ // insert last line...
		vec_str.push_back(str_line);
	}

	clo.n_lines=vec_str.size();
	clo.text=(char **)malloc(sizeof(char *)*vec_str.size());

	for(unsigned i=0; i < vec_str.size(); i++){
		unsigned len =vec_str[i].size();
		clo.text[i]=(char *)malloc(sizeof(char)*(len+1)); // +1 for final str char...
		memset(clo.text[i],0,sizeof(char)*(len+1)); // fill with space char ...
		strncpy(clo.text[i],vec_str[i].c_str(),len); // copy string...
	}

	console_line_output.push_back(clo);
	return &console_line_output[console_line_output.size()-1];
}

void CConsole::drawChar(int x,int y, char c_text, CFont *font,int rgb){
	if(font){
		SDL_Texture *font_text=font->getTexture();
		if(font_text){
			SDL_Rect rect_charout={x,y,font->getCharWidth(),font->getCharHeight()};
			SDL_SetTextureColorMod(font_text,
					 rgb&0xFF,
				   (rgb>>8)&0xFF,
				   (rgb>>16)&0xFF);

			SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c_text), &rect_charout);

			rect_charout.x+=rect_charout.w;
			if(rect_charout.x>=CONSOLE_WIDTH){
				rect_charout.x=0;
				rect_charout.y+=rect_charout.h;
			}
		}
	}
}

SDL_Rect *CConsole::drawText(int x,int y, const char * c_text, CFont *font, int rgb, unsigned int properties){
	if(font){
		SDL_Texture *font_text=font->getTexture();
		if(font_text){

			if(properties & CENTER_TEXT_PROPERTY){ // only works with texts without carry returns...
				x-=(font->getCharWidth()*strlen(c_text))/2;
			}

			rect_textout={x,y,font->getCharWidth(),font->getCharHeight()};
			SDL_SetTextureColorMod(font_text,
									 rgb&0xFF,
								    (rgb>>8)&0xFF,
								    (rgb>>16)&0xFF);

			for(unsigned i=0; i < strlen(c_text); i++){
				char c=c_text[i];
				int repeat_char=1;
				//int w_char=font->getCharWidth();

				if(c=='\r') // ignore ...
					continue;

				if(c=='\n'){ // ignore ...
					rect_textout.x=0;
					rect_textout.y+=rect_textout.h;
					continue;
				}

				/*if(c=='\t'){ // advance next char by 4 spaces...
					c=' ';
					repeat_char=TAB_CHAR_WIDTH;
				}*/

				for(int r=0; r < repeat_char; r++){

					SDL_RenderCopy(pRenderer, font_text, font->getRectChar(c), &rect_textout);
					rect_textout.x+=rect_textout.w;
					if(rect_textout.x>=CONSOLE_WIDTH){
						rect_textout.x=0;
						rect_textout.y+=rect_textout.h;
					}
				}
			}
			// correct offset as needed...
			if(((rect_textout.x+font->getCharWidth())>=CONSOLE_WIDTH) && (properties & WRAP_WINDOW_PROPERTY)){ // carry return ...
				rect_textout.y+=rect_textout.h;
				rect_textout.x=0;
			}

			return &rect_textout;
		}
	}
	return NULL;
}

void CConsole::toggleFullscreen(){
	if(!fullscreen){
		SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN);
	}else{
		SDL_SetWindowFullscreen(pWindow, 0);
	}

	fullscreen=!fullscreen;
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
	tConsoleLineOutput * clo =print(text_out);
	clo->rgb=RMASK32;
}

void CConsole::printOut(const char *text_in,...){
	char text_out[4096];
	va_list  ap;
	va_start(ap,  text_in);
	vsprintf(text_out,  text_in,  ap);
	va_end(ap);
	print(text_out);
}

int CConsole::getOffsetConsolePrint(int & intermid_line){

	//int n_lines = 0;
	intermid_line=0;
	int offset = console_line_output.size()-1;
	int n_lines_output=N_LINES_TEXT_WRAP(output);
	int n_lines=0;

	if(offset>=0){

		do{
			n_lines+=console_line_output[offset--].n_lines;

		}while(offset>=0 && ((n_lines+n_lines_output) < CHARS_PER_HEIGHT));

		// correct offset...
		offset++;

		if((n_lines+n_lines_output)>=CHARS_PER_HEIGHT){
			intermid_line=(n_lines+n_lines_output)-CHARS_PER_HEIGHT;
			if(intermid_line==0){ // no lines but
				//offset++;
			}
		}
	}
	return offset;
}

void CConsole::toggleApplicationPopup(){
	edit_copy_popup_open=!edit_copy_popup_open;
	if(edit_copy_popup_open){
		xpopup=xmouse;
		ypopup=ymouse;
	}
}

void CConsole::copyText(){

	if(line_ini< line_end || col_ini < col_end){
		string copy="";//console_text.substr(start_select_char,end_select_char);
		int end=MIN((int)(console_text.size()-1),line_end);
		copy+=console_text[line_ini].substr(col_ini);

		for(int i = line_ini+1; i < (end);i++){//MIN(line_end,console_text.size()); i++){
			copy+=console_text[i];
		}

		if(line_ini< end){
			copy+=console_text[end].substr(0,col_end);
		}

		line_ini=-1;
		line_end=-1;
		col_ini=-1;
		col_end=-1;

		SDL_SetClipboardText(copy.c_str());
		alert(1000,"Text copied");
	}

	if(edit_copy_popup_open){
		edit_copy_popup_open=false;
	}
}

void CConsole::pasteText(){
	if(edit_copy_popup_open){
		edit_copy_popup_open=false;
	}

	if(SDL_HasClipboardText()){
		char * aux=SDL_GetClipboardText();
		string partial_out="";
		size_t len = strlen(aux);
		for(size_t i=0; i < len; i++){
			if(aux[i]=='\t'){ // replace tabs by 4 chats
				for(int i=0; i < 4; i++){
					partial_out+=' ';
				}
			}
			else if(aux[i]!='\r'){
				partial_out+=aux[i];
			}
		}
		output.insert(MIN((int)output.size(),char_cursor),partial_out);
		char_cursor+=partial_out.size();

		//printf("cursor %i\n",char_cursor);

		alert(1000,"Text paste");
	}
}

void CConsole::updateApplicationPopup(){
	if(edit_copy_popup_open){
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

		y+=5;
		drawText(x+(POPUP_WIDTH>>1),y,"Copy",popup_font,(((line_ini < line_end) && (col_ini < col_end)))?(MOUSE_COLLIDE_COPY_ITEM?0x00FF00:0xFFFFFF):0x1F1F1F,CENTER_TEXT_PROPERTY);
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

	drawText((CONSOLE_WIDTH>>1),(CONSOLE_HEIGHT>>1)-(popup_font->getCharHeight()>>1),text_alert,popup_font,-1,CENTER_TEXT_PROPERTY);
}

const char * CConsole::update(){
	bool cr=false;
	bool typed_input=false;

	// clear screen...
	clear(0,0,0);

	if(line_ini>=0 && (line_ini< line_end || col_ini < col_end)){// && col_ini <= col_end){//(x1_sel < x2_sel) && (y1_sel < y2_sel )){

		int x1_sel=col_ini*CHAR_WIDTH;
		int y1_sel=line_ini*CHAR_HEIGHT;

		int x2_sel=(col_end+1)*CHAR_WIDTH;
		int y2_sel=(line_end+1)*CHAR_HEIGHT;

		SDL_SetRenderDrawColor( pRenderer, 0x1F, 0x1F, 0x1F, 0x1F );

		// render first line..
		SDL_Rect fillRect = {
				  x1_sel
				, y1_sel
				, line_ini < line_end ? CONSOLE_WIDTH-x1_sel:(x2_sel-x1_sel)
				, console_font->getCharHeight() };
		SDL_RenderFillRect( pRenderer, &fillRect );

		// render middle..
		for(int l=line_ini+1;l < (line_end); l++){
			fillRect = {
							  0
							, (l)*console_font->getCharHeight()
							, CONSOLE_WIDTH
							, console_font->getCharHeight() };
			SDL_RenderFillRect( pRenderer, &fillRect );
		}

		// render last line..
		if(line_ini < line_end){
		 fillRect = {
				  0
				, y2_sel-console_font->getCharHeight()
				, x2_sel
				, console_font->getCharHeight() };
		 SDL_RenderFillRect( pRenderer, &fillRect );
		}

	}

	// update keyboard events...
	int intermid=0;
	int offset=getOffsetConsolePrint(intermid);
	int offsetY=0;
	SDL_Rect * rect=0;
	int lines=0;
	const char *output_start=output.c_str();
	console_text.clear();//="";

	for(unsigned i=offset; i < console_line_output.size(); i++){
		for(int l=((int)i==offset)?intermid:0; l < console_line_output[i].n_lines; l++){
			rect=drawText(0,offsetY,console_line_output[i].text[l],console_font,console_line_output[i].rgb);
			console_text.push_back(console_line_output[i].text[l]);
			offsetY+=rect->h;
			lines++;
		}
	}

	int starting_line=N_LINES_TEXT_WRAP(output);
	if((starting_line)>(CHARS_PER_HEIGHT)){
		starting_line-=(CHARS_PER_HEIGHT);
		output_start=output.c_str()+CHARS_PER_WIDTH*starting_line;
	}

	int chars_left=strlen(output_start);
	char *ptr=(char *)output_start;
	char ch_copy[CHARS_PER_WIDTH+1];//={0};
	while(chars_left>0){

		memset(ch_copy,0,CHARS_PER_WIDTH+1);
		int len = CHARS_PER_WIDTH;
		if(chars_left < len){
			len = chars_left;
		}
		strncpy(ch_copy,ptr,len);

		console_text.push_back(string(ch_copy));

		chars_left-=len;
		ptr+=len;
	}

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
						//if(start_select_char < end_select_char){
						if(MOUSE_COLLIDE_COPY_ITEM){ // copy selected text
							copyText();
						}if(MOUSE_COLLIDE_PASTE_ITEM){
							pasteText();
						}else{
							edit_copy_popup_open=false;
						}
					}
					select_start_line=line_ini= e.button.y/CHAR_HEIGHT;
					select_start_col=col_ini=e.button.x/CHAR_WIDTH;

					line_end =  line_ini;
					col_end = col_ini;

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

					int line = e.button.y/CHAR_HEIGHT;
					int col = e.button.x/CHAR_WIDTH;

					line_ini=MIN(select_start_line,line);
					col_ini=MIN(select_start_col,col);

					line_end=MAX(select_start_line,line);
					col_end=MAX(select_start_col,col);
				}
				break;
			case SDL_KEYDOWN:

				switch(e.key.keysym.sym){
					case 	KEY_v:
						if(SDL_GetModState() & KMOD_CTRL){
							pasteText();
						}
						break;
					case 	KEY_c:
						if(SDL_GetModState() & KMOD_CTRL){
							copyText();
						}
						break;
					case SDLK_RETURN:
						if(SDL_GetModState() & KMOD_SHIFT){ // just put '\n
							output.insert(output.begin()+char_cursor,'\n');
							//output+="\n";
							char_cursor++;//=output.size();
							//char_cursor++;
						}
						else{
							cr=true;
						}
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
					case SDLK_HOME: // begin...
						char_cursor=prompt.size();
						break;
					case SDLK_END: // end...
						char_cursor = output.size();
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
					case SDLK_BACKSPACE:
						if((unsigned)char_cursor>prompt.size()){
							output.erase(output.begin()+(char_cursor-1));
							char_cursor--;
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

	if(cr){

		//output+="\n";
		print(output.c_str());
		str_type_input=(char *)(output.c_str()+prompt.size());
		cr=false;
		typed_input = true;

		history.push_back(str_type_input);
		history_cursor=history.size();
		setOutput("");

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
