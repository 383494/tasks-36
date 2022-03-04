// 2022/2/20

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<stdio.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void drawpol(int x, int y, int r, int sides){
	SDL_Point* pts = (SDL_Point*)SDL_malloc(sizeof(SDL_Point)*(sides+1));

	for(int i=0;i<=sides;i++){
		pts[i].x =(int)( x + r*SDL_cosf(PI*2*i/sides-PI/2) +0.5 );
		pts[i].y =(int)( y + r*SDL_sinf(PI*2*i/sides-PI/2) +0.5 );
	}
	SDL_RenderDrawLines(ren,pts,sides+1);
	SDL_free(pts);
}

void DrawText(SDL_Surface* screen, TTF_Font* font, const char* text, SDL_Rect rect)
{
	SDL_Color white={255,255,255};
	SDL_Surface* textSfc=NULL;
	textSfc=TTF_RenderText_Solid(font,text,white);
//	if(!text)printf("%s\n",TTF_GetError());
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface( ren, textSfc );
//	if(!text_texture)printf("%s",SDL_GetError());
	SDL_Rect dest = { 0, 0, textSfc->w, textSfc->h };
	rect.w=textSfc->w;
	rect.h=textSfc->h;
	SDL_FreeSurface(textSfc);
	SDL_RenderCopy( ren, text_texture, &dest ,&rect);
	SDL_DestroyTexture(text_texture);
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

	if(TTF_Init())puts("ttf error");
	
	winSurface = SDL_GetWindowSurface(win);

	TTF_Font *font;
	SDL_Surface* text=NULL;
	font=TTF_OpenFont("./simsun.ttc",30);
	if(!font)printf("%s\n",TTF_GetError());
	TTF_SetFontStyle(font,TTF_STYLE_NORMAL);

	/// start drawing
	SDL_Rect rect={80,80,0,0};

	char num[3];
	SDL_bool looping=SDL_TRUE;
	int i = 3;
	while(looping){
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT){
				//puts("stop");
				looping = SDL_FALSE;
				break;
			}
		}
		SDL_SetRenderDrawColor(ren,0,0,0,255);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren,255,255,255,255);
		sprintf(num,"%d",i);
		DrawText(winSurface, font, num, rect);
		drawpol(100,100,100,i);
		SDL_RenderPresent(ren);
		i++;
		if(i>50){
			looping=SDL_FALSE;
			break;
		}
		SDL_Delay(200);
	}
//	for(int i=3;i<=50 && (!shouldstop) ;i++){
		
//	}



	/// end drawing

	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);



	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(text);

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
