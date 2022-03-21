// 2022/3/1

#include<SDL2/SDL.h>
//#include<SDL2/SDL_ttf.h>
#include<stdio.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void drawpol(float x, float y, float r, int sides){
	SDL_FPoint pts[2];
	for(int i=0;i<=sides;i++){
		pts[i%2].x = x + r * SDL_cosf(PI*2*i/(float)sides-PI/2.0);
		pts[i%2].y = y + r * SDL_sinf(PI*2*i/(float)sides-PI/2.0);
		SDL_RenderDrawLinesF(ren,pts,2);
	}
}

void quitall(){
	if(winSurface)
		SDL_FreeSurface(winSurface);
	if(tex)
		SDL_DestroyTexture(tex);
	if(ren)
	    SDL_DestroyRenderer(ren);
	if(win)
	    SDL_DestroyWindow(win);
    SDL_Quit();
}

#ifdef SDL_TTF_H_
void DrawText(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y)
{
	SDL_Color white={255,255,255};
	SDL_Surface* textSfc=NULL;
	textSfc=TTF_RenderUTF8_Solid(font,text,white);
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

void drawcircle(int x, int y, int r){
	int ody=0;//old dy
	int dy=0;
	for(int curx=x-r+1;curx<=x+r;curx++){
		dy=SDL_floor(0.5 + SDL_sqrt( (r+curx-x) * (r-curx+x) ) );
		SDL_RenderDrawLine(ren,curx,y+dy,curx-1,y+ody);
		SDL_RenderDrawLine(ren,curx,y-dy,curx-1,y-ody);
		ody=dy;
	}
}

void fillcircle(int x, int y, int r){
	drawcircle(x,y,r);
	for(int curx=x-r;curx<=x+r;curx++){
		int dy=SDL_floor(0.5 + SDL_sqrt( (r+curx-x) * (r-curx+x) ) );
		SDL_RenderDrawLine(ren,curx,y+dy,curx,y-dy);
	}
}

SDL_bool insideRect(int x, int y, SDL_Rect rect){
    return (x>=rect.x && x<=rect.x+rect.w && y>=rect.y && y<=rect.y+rect.h);
}

int main(int argc, char *argv[])
{
	/// initalize sdl
	if(SDL_Init(SDL_INIT_VIDEO)){
		printf("Error to init: %s",SDL_GetError());
		quitall();
		return 1;
	}

	/// create window
	win = SDL_CreateWindow("Title"
		, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
		, 640, 480, SDL_WINDOW_SHOWN);
	if(!win){
		printf("Error to create window: %s",SDL_GetError());
		quitall();
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, 0);
	if(!ren){
		printf("Error to create renderer: %s",SDL_GetError());
		quitall();
		return 1;
	}

	winSurface = SDL_GetWindowSurface(win);
	if(!winSurface){
		printf("Error to get surface: %s",SDL_GetError());
		quitall();
		return 1;
	}
	/// start drawing

	//srand((unsigned int)SDL_GetTicks());

	int queen[9]={0};
	int quno=1;
	SDL_bool solveflag = SDL_FALSE;
	
	unsigned int tick = 0;
    SDL_bool looping = SDL_TRUE;
	while(looping){
		if(tick%2==0){
			SDL_SetRenderDrawColor(ren, 255, 255, 255,255);
			SDL_RenderClear(ren);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			queen[quno]++;
			int i=quno;
			SDL_bool canplace = queen[quno]<9;
			while(--i){
				canplace &= queen[i]!=queen[quno];			//不在一列
				canplace &= queen[i]-i!=queen[quno]-quno;	//不在一斜线
				canplace &= queen[i]+i-queen[quno]-quno!=0; 
				if(!canplace){
					SDL_RenderDrawLine(ren, queen[quno]*50-30, quno*50-30,queen[i]*50-30,i*50-30);
					break;
				}
			}
			if(canplace){
				if(++quno == 9){				//放完了

					//输出结果
					/*
					i = 0;
					while(++i <= 8){
						printf("%d",queen[i]);
						//queen[i] = 0;
					}
					printf("\t");
					//*/\
					solveflag = SDL_TRUE;

				}
			}
			else {
				if(queen[quno] >= 9){			//放不下了
					queen[quno--] = 0;			//本行置0并返回上一行
					if(quno == 0){				//结束
						looping = SDL_FALSE;
						break;
					}
				}
			}
			if(solveflag)SDL_SetRenderDrawColor(ren, 127,0,255,255);
			SDL_Rect rect = {0,0,40,40};
			for(i=quno;i>0;i--){
				//printf("%d",queen[i]);
				rect.x = queen[i]*50-50;
				rect.y = i*50-50;
				SDL_RenderFillRect(ren, &rect);
			}
			SDL_RenderPresent(ren);
			if(solveflag){
				SDL_Delay(1000);
				solveflag = SDL_FALSE;
			}
			//printf("\t");
		}


		SDL_Delay(20);
		tick++;
		while(SDL_PollEvent(&ev)){
			switch(ev.type){
				case SDL_QUIT:
					looping = SDL_FALSE;
					break;
				
			}
		}
	}

	/// end drawing

//	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);

	quitall();

	return 0;
}

