/// 最后修改:2021/2/18

#include<SDL2/SDL.h>
#include<math.h>
#include<stdio.h>
#undef main

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
	
	/// get surface
//	winSurface = SDL_GetWindowSurface(win);
//	if(!winSurface){
//		printf("Error to get surface: %s",SDL_GetError());
//		return 1;
//	}

	ren = SDL_CreateRenderer(win, -1, 0);
	if(!ren)puts("error ren");

	double sq2=sqrt(2.0);
	int sq2i=(int)round(25*sq2);

	SDL_SetRenderDrawColor(ren,12,128,0,255);
	drawtri(0,4*sq2i+100,sq2i,5*sq2i+100,2*sq2i,SDL_TRUE);
	drawtri(sq2i,5*sq2i+100,2*sq2i,4*sq2i+100,2*sq2i,SDL_TRUE);

	SDL_SetRenderDrawColor(ren,233,0,128,255);
	drawtri(3*sq2i,5*sq2i+100,3*sq2i,5*sq2i,100,SDL_TRUE);

	SDL_SetRenderDrawColor(ren,5,233,0,255);
	drawtri(5*sq2i,3*sq2i,3*sq2i,5*sq2i,4*sq2i,SDL_FALSE);

	SDL_SetRenderDrawColor(ren,45,66,250,255);
	drawtri(5*sq2i,3*sq2i,5*sq2i+50,3*sq2i+50,100,SDL_FALSE);

	SDL_SetRenderDrawColor(ren,10,200,200,255);
	drawtri(4*sq2i,2*sq2i,5*sq2i,sq2i,2*sq2i,SDL_TRUE);
	drawtri(4*sq2i,2*sq2i,5*sq2i,3*sq2i,2*sq2i,SDL_TRUE);

	SDL_SetRenderDrawColor(ren,233,40,60,255);
	drawtri(4*sq2i,0,5*sq2i,sq2i,2*sq2i,SDL_FALSE);

	SDL_SetRenderDrawColor(ren,99,40,233,255);
	drawtri(6*sq2i,0,5*sq2i,sq2i,2*sq2i,SDL_FALSE);

	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);
	
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
