// 2021/2/18

#include<SDL2/SDL.h>
#include<stdio.h>
#include<math.h>
#undef main
#define PI 3.1415927

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void drawtri(int x, int y, int x0, int y0, int d, SDL_bool dx);

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

	SDL_FPoint pts[6]={};

	for(int i=0;i<=6;i++){
		pts[i].x=100+100*cosf(PI*4*i/5-PI/2);
		pts[i].y=100+100*sinf(PI*4*i/5-PI/2);
	}

	SDL_SetRenderDrawColor(ren,255,255,255,255);

	SDL_RenderDrawLinesF(ren,pts,6);

	SDL_RenderPresent(ren);
	
	while(SDL_WaitEvent(&ev)){
		if(ev.type==SDL_QUIT)break;
	}

	SDL_FreeSurface(winSurface);
	SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

	return 0;
}

void drawtri(int x, int y, int x0, int y0, int d, SDL_bool dx){//不会填充就只能这样画三角了
	d++;
	if(dx){
		while(d--){
			SDL_RenderDrawLine(ren, x0, y0, x+d, y);
		}
	}
	else{
		while(d--){
			SDL_RenderDrawLine(ren, x0, y0, x, y+d);
		}
	}
}
