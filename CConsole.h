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

	static CConsole *console;
	CInput *input;
	Uint32 start_ms;
	bool is_blink;

	int currentX,currentY;
	std::string partial_str;

	int CONSOLE_WIDTH;
	int CONSOLE_HEIGHT;
	short CHARS_PER_WIDTH;
	short CHARS_PER_HEIGHT;
	unsigned total_lines;

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

	tConsoleLineOutput * print(const char *c);
	int getOffsetConsolePrint(int & intermid_line);
	unsigned getTotalLines();

	CConsole();
	~CConsole();

public:

	static CConsole *getInstance();
	static void destroy();



	void init(int width, int height);

	void setFont(CFont * font);
	void toggleFullscreen();
	SDL_Renderer * getRenderer();

	int getWidth();
	int getHeight();




	void printError(const char *c);
	void printOut(const char *c);

	bool update();

};
