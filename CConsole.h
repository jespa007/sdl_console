/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#include "CFont.h"
#include "CInput.h"
#include   <SDL2/SDL.h>

class CConsole{

	struct tConsoleLineOutput{
		char ** text;
		int n_lines;
		Uint32 rgb;

		tConsoleLineOutput(){
			text=NULL;
			n_lines=1;
			rgb=-1;
		}
	};

	std::vector<tConsoleLineOutput> console_line_output;
	std::vector<string> history;

	static CConsole *console;
	CInput *input;
	Uint32 start_ms;
	string prompt;
	bool is_blink;

	int currentX,currentY;
	std::string partial_str;

	bool selecting;
	int start_select_char;
	int end_select_char;
	//Render red filled quad
	int x1_sel,y1_sel;
	int x2_sel,y2_sel;

	int CONSOLE_WIDTH;
	int CONSOLE_HEIGHT;
	short CHARS_PER_WIDTH;
	short CHARS_PER_HEIGHT;

	bool quit;
	string str_type_input;
	int char_cursor;
	int history_cursor;

	CFont *font;

	SDL_Rect rect_textout;

	std::string output;

	SDL_Event e;
	//std::string current_line;

	//SDL_Surface* pScreenSurface = NULL;
	SDL_Renderer *pRenderer = NULL;
	SDL_Event event;
	bool fullscreen;

	SDL_Window* pWindow = NULL;


	bool blink_500ms();

	void clear(Uint8 r, Uint8 g, Uint8 b);
	void setColor(Uint8 r, Uint8 g, Uint8 b);
	SDL_Rect * drawText(int x,int y,  const char * text, int rgb=-1);
	void drawChar(int x,int y,  const char c, int rgb=-1);
	SDL_Rect *getCurrentCursor(int x,int y, const char * c_text);
	tConsoleLineOutput * print(const char *c);
	int getOffsetConsolePrint(int & intermid_line);
	unsigned getTotalLines();
	void setOutput(const string & str);

	CConsole();
	~CConsole();

public:

	static CConsole *getInstance();
	static void destroy();



	void init(int col_width, int col_height);

	void setFont(CFont * font);
	void toggleFullscreen();
	void setPrompt(const char *_prompt);


	SDL_Renderer * getRenderer();

	int getWidth();
	int getHeight();





	void printError(const char *c,...);
	void printOut(const char *c,...);

	const char * update();
	bool end();

};
