// 2022/2/21

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

    float bx,by,vx,vy;
	srand((unsigned int)&bx);//使用内存地址实现随机化
    bx=50;
    by=rand()%(rect.h-50)+25;
    vx=rand()/(float)RAND_MAX;vx*=30;
    vy=rand()/(float)RAND_MAX;vy*=-30;
	printf("vx:%lf,vy:%lf\n",vx,vy);
	SDL_bool looping=SDL_TRUE;
	
	while(looping){

        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderDrawRect(ren,&rect);
        drawcircle(bx, by, 20);
    	SDL_RenderPresent(ren);
		bx+=vx;
    	by+=vy;
		if(rect.y+rect.h-by-24<=1.5 && SDL_abs(vy)<=5) {
			vy*=0.9;
		}
		else {
			vy+=1;
		}
        if(bx-25<=rect.x||bx+25>=rect.x+rect.w//检测碰撞
            ){
            vx*=-0.7;bx+=vx;
			printf("vx:%lf\n",vx);
        }else if(by+25>=rect.y+rect.h||by-25<=rect.y){
            vy*=-0.7;by+=vy;//vy+=1;
			printf("vy:%lf\n",vy);
        }

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

