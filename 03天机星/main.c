/// 网格:(2*15)*(2*10) ,每个小方格20*20px

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<math.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void drawstar(int ox, int oy,int r, float theta){//ox oy是圆心的xy r是外接圆半径 theta是整体旋转的角度
	SDL_FPoint pts[6]={};
	SDL_RenderDrawLine(ren,100,100,ox,oy);

	for(int i=0;i<=6;i++){
		pts[i].x=ox+r*cosf(PI*4*i/5-PI/2-theta);
		pts[i].y=oy+r*sinf(PI*4*i/5-PI/2-theta);
	}
	for(theta=PI*2;theta>=0;theta-=PI/180){
		SDL_RenderDrawPointF(ren,ox+r*cosf(theta),oy+r*sinf(theta));
	}

	SDL_RenderDrawLinesF(ren,pts,6);
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

	/// start drawing

	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderClear(ren);

	SDL_SetRenderDrawColor(ren,255,0,0,255);
	for(int y=0;y<=2;y++){
		SDL_RenderDrawLine(ren,0,y*200,600,y*200);
	}
	for(int x=0;x<=2;x++){
		SDL_RenderDrawLine(ren,x*300,0,x*300,400);
	}
	for(int y=0;y<=10;y++){
		SDL_RenderDrawLine(ren,0,y*20,300,y*20);
	}
	for(int x=0;x<=15;x++){
		SDL_RenderDrawLine(ren,x*20,0,x*20,200);
	}

	SDL_SetRenderDrawColor(ren,0,0,0,255);
	drawstar(100,100,60,0);
	drawstar(200,40,20,(float)PI-atan(5.0/3.0));
	drawstar(240,80,20,(float)PI-atan(7.0/1.0));
	drawstar(240,140,20,(float)atan(7.0/2.0));
	drawstar(200,180,20,(float)atan(5.0/4.0));

	/// end drawing
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
