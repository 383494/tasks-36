// 2022/2/20

#include<SDL2/SDL.h>
#include<stdio.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void drawpol(float x, int y, int r, int sides){
	SDL_FPoint* pts = (SDL_FPoint*)SDL_malloc(sizeof(SDL_FPoint)*(sides+1));

	for(int i=0;i<=sides;i++){
		pts[i].x = x + r * SDL_cosf(PI*2*i/(float)sides-PI/2.0);
		pts[i].y = y + r * SDL_sinf(PI*2*i/(float)sides-PI/2.0);
	}
	SDL_RenderDrawLinesF(ren,pts,sides+1);
	SDL_free(pts);
}


void drawcircle(float x, float y, float r){
	drawpol(x,y,r,(int)3*SDL_ceil(SDL_exp(SDL_log(r/3.0))));
}

void fillcircle(float x, float y, float r){
	for(;r>0;r-=0.5){
		drawcircle(x,y,r);
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

    int bx,by,bangle;
	srand((unsigned int)&bx);//使用内存地址实现随机化
    bx=(rand()%(rect.w-40))+rect.x+20;
    by=(rand()%(rect.h-40))+rect.y+20;
    bangle=((rand()%4)*90)+45;

	SDL_bool looping=SDL_TRUE;
//	int i = 3;
	while(looping){
        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren,255,255,255,255);
        SDL_RenderDrawRect(ren,&rect);
        fillcircle(bx, by, 20);
        SDL_RenderPresent(ren);
        bx+=(int)(10*SDL_cos(PI/180*bangle));
        by+=(int)(10*SDL_sin(PI/180*bangle));
        if(bx-25<=rect.x||bx+25>=rect.x+rect.w//检测碰撞
            ){
            bangle=180-bangle;
            bangle%=360;
//			printf("%d\n",bangle);
        }else if(by+25>=rect.y+rect.h||by-25<=rect.y){
            bangle=-bangle;
			bangle%=360;
//			printf("%d\n",bangle);
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

