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

void drawcircle(int x, int y, int r){
    for(int angle=0; angle<360; angle++){
        SDL_RenderDrawLine(ren, x, y, x + r * SDL_cos(PI/180*angle),y + r * SDL_sin(PI*angle/180));
    }
}

struct Ballstr{
	int x;
	int y;
	int angle;
};

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
    
    SDL_Rect rect={10,10,600,400};
//    SDL_RenderDrawLine(ren, 10, 10, 610, 410);

    struct Ballstr balls[10];
	srand((unsigned int)balls);
	for(char i=0;i<10;i++){
		balls[i].x=(rand()%(rect.w-40))+rect.x+20;
		balls[i].y=(rand()%(rect.h-40))+rect.y+20;
		balls[i].angle=((rand()%4)*90)+45;
	}
	SDL_bool looping=SDL_TRUE;
	while(looping){
		SDL_SetRenderDrawColor(ren,0,0,0,255);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderDrawRect(ren,&rect);
		for(char i=0;i<10;i++){
			drawcircle(balls[i].x, balls[i].y, 20);
			balls[i].x+=(int)(10*SDL_cos(PI/180*balls[i].angle));
			balls[i].y+=(int)(10*SDL_sin(PI/180*balls[i].angle));
			if(balls[i].x-25<=rect.x||balls[i].x+25>=rect.x+rect.w//检测碰撞
				){
				balls[i].angle=180-balls[i].angle;
				balls[i].angle%=360;
			}else if(balls[i].y+25>=rect.y+rect.h||balls[i].y-25<=rect.y){
				balls[i].angle=-balls[i].angle;
				balls[i].angle%=360;
			}
		}
		SDL_RenderPresent(ren);

		SDL_Delay(20);
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT){
				//puts("stop");
				looping = SDL_FALSE;
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

