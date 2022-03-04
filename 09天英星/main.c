// 2022/2/22

#include<SDL2/SDL.h>
//#include<SDL2/SDL_ttf.h>
#include<stdio.h>
#include<time.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

#ifdef SDL_TTF_H_

void DrawText(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y, SDL_Renderer* ren, Uint8 R, Uint8 G, Uint8 B)
{
	SDL_Color textcolor={R,G,B};
	SDL_Surface* textSfc=NULL;
	textSfc=TTF_RenderText_Solid(font,text,textcolor);
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface( ren, textSfc );
	SDL_Rect dest = { 0, 0, textSfc->w, textSfc->h };
	SDL_Rect rect;
	rect.x=x;
	rect.y=y;
	rect.w=textSfc->w;
	rect.h=textSfc->h;
	SDL_FreeSurface(textSfc);
	SDL_RenderCopy( ren, text_texture, &dest ,&rect);
	SDL_DestroyTexture(text_texture);
}

#endif

void drawpol(int x, int y, int r, int sides){
	SDL_Point* pts = (SDL_Point*)SDL_malloc(sizeof(SDL_Point)*(sides+1));

	for(int i=0;i<=sides;i++){
		pts[i].x =(int)( x + r*SDL_cosf(PI*2*i/sides-PI/2) +0.5 );
		pts[i].y =(int)( y + r*SDL_sinf(PI*2*i/sides-PI/2) +0.5 );
	}
	SDL_RenderDrawLines(ren,pts,sides+1);
	SDL_free(pts);
}

int main(int argc, char *argv[])
{
	/// initalize sdl
	if(SDL_Init(SDL_INIT_VIDEO)){
		printf("Error to init: %s",SDL_GetError());
		return 1;
	}

	/// create window
	win = SDL_CreateWindow("Title"
		, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
		, 640, 480, SDL_WINDOW_SHOWN);
	if(!win){
		printf("Error to create window: %s",SDL_GetError());
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, 0);
	if(!ren)puts("error ren");

	winSurface = SDL_GetWindowSurface(win);
	/// start drawing

#ifdef SDL_TTF_H_

    if(TTF_Init())puts("ttf init error");
    TTF_Font *font=TTF_OpenFont("./simsun.ttc",30);
	SDL_Surface* text=NULL;
	if(!font)printf("%s\n",TTF_GetError());
	TTF_SetFontStyle(font,TTF_STYLE_NORMAL);

#endif

	struct tm *curtm;
	time_t curtm_t;
    SDL_bool looping=SDL_TRUE;
	
	while(looping){

		SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderClear(ren);

		SDL_SetRenderDrawColor(ren,0,0,0,255);

		drawpol(320,240,240,360);
		for(char i=0;i<12;i++){
			SDL_RenderDrawLineF(ren, 320+230*SDL_cosf(PI/6.0*i), 240+230*SDL_sinf(PI/6.0*i),
				320+240*SDL_cosf(PI/6.0*i), 240+240*SDL_sinf(PI/6.0*i));
		}

		time(&curtm_t);
		curtm=localtime(&curtm_t);

		SDL_RenderDrawLineF(ren,320.0,225.0,320+225*SDL_cosf(PI/30.0*curtm->tm_sec),240+225*SDL_sinf(PI/30.0*curtm->tm_sec));
		SDL_RenderDrawLineF(ren,320.0,225.0,320+190*SDL_cosf(PI/30.0*curtm->tm_min),240+190*SDL_sinf(PI/30.0*curtm->tm_min));
		SDL_RenderDrawLineF(ren,320.0,225.0,320+140*SDL_cosf(PI/30.0*curtm->tm_hour),240+140*SDL_sinf(PI/30.0*curtm->tm_hour));
		

		SDL_RenderPresent(ren);

		SDL_Delay(97);
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT){
				looping=SDL_FALSE;
				break;
			}
		}
	}

#ifdef  SDL_TTF_H_

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(text);
	
#endif

	/// end drawing

//	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);

	if(winSurface)
		SDL_FreeSurface(winSurface);
	if(tex)
		SDL_DestroyTexture(tex);
	if(ren)
	    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

	return 0;
}

