// 2022/2/22

#include<SDL2/SDL.h>
#include<stdio.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void drawcircle(float x, float y, int r){
    for(int angle=0; angle<360; angle++){
        SDL_RenderDrawLineF(ren, x, y, x + r * SDL_cos(PI/180.0*angle),y + r * SDL_sin(PI*angle/180.0));
    }
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

    

	SDL_bool looping=SDL_TRUE;
	float ex,ey;//e==earth
	short ea,ma;//m==moon,a==angle
	ea=0;ma=0;
	while(looping){
		SDL_SetRenderDrawColor(ren,255,255,255,255);
  		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren,0,255,0,255);
		ex=320+200*SDL_cosf(ea*PI/180.0);
		ey=240+150*SDL_sinf(ea*PI/180.0);
		drawcircle(ex,ey,30);
		SDL_SetRenderDrawColor(ren,255,255,0,255);
		drawcircle(320,240,50);
		drawcircle(ex+100*SDL_cosf(ma*PI/180.0),ey+75*SDL_sinf(ma*PI/180.0),15);
		ea++;ea%=360;
		ma+=2;ma%=360;
		SDL_RenderPresent(ren);
		SDL_Delay(20);
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT){
				looping=SDL_FALSE;
				break;
			}
		}
	}

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

