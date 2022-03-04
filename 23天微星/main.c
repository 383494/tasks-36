// 2022/2/28

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
/*
void snow(int x, int y, unsigned int layer, float l, int angle){
	SDL_FPoint pts[5];
	pts[0].x=x, pts[0].y=y;
	pts[4].x=x+l*SDL_cos(PI/180.0*angle), pts[4].y=y+l*SDL_sin(PI/180.0*angle);
	pts[1].x=(2*x+pts[4].x)/3.0, pts[1].y=(2*y+pts[4].y)/3.0;
	pts[3].x=(x+2*pts[4].x)/3.0, pts[3].y=(y+2*pts[4].y)/3.0;
	pts[2].x=x+l/SDL_sqrt(3.0)*SDL_cos(PI/180.0*angle+PI/6);
	pts[2].y=y+l/SDL_sqrt(3.0)*SDL_sin(PI/180.0*angle+PI/6);
	for(int i=0;i<5;i++)pts[i].y=480-pts[i].y;
	//printf("%lf,%lf",pts[0].y,pts[4].y);
	SDL_SetRenderDrawColor(ren, 255,255,255,255);
	SDL_RenderDrawLineF(ren, pts[1].x,pts[1].y,pts[3].x,pts[3].y);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderDrawLinesF(ren, pts,5);
	if(layer==0)return;
	else snow(x,y,layer-1,l/3.0,angle);
}*/

void snow(SDL_FPoint pta, SDL_FPoint ptb, unsigned int layer){
	SDL_FPoint pts[5];
	pts[0]=pta;
	pts[4]=ptb;
	pts[1].x=(2*pts[0].x+pts[4].x)/3.0, pts[1].y=(2*pts[0].y+pts[4].y)/3.0;
	pts[3].x=(pts[0].x+2*pts[4].x)/3.0, pts[3].y=(pts[0].y+2*pts[4].y)/3.0;
	pts[2].x=(pts[0].x+pts[4].x)/2.0+(pts[0].y-pts[4].y)/2.0/SDL_sqrt(3);
	pts[2].y=(pts[0].y+pts[4].y)/2.0-(pts[0].x-pts[4].x)/2.0/SDL_sqrt(3);
	
	if(layer==0){
		//SDL_SetRenderDrawColor(ren, 255,255,255,255);
		//SDL_RenderDrawLineF(ren, pts[1].x,pts[1].y,pts[3].x,pts[3].y);
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderDrawLinesF(ren, pts,5);
	}else{
		snow(pts[0],pts[1],layer-1);
		snow(pts[1],pts[2],layer-1);
		snow(pts[2],pts[3],layer-1);
		snow(pts[3],pts[4],layer-1);
	}
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
	SDL_SetRenderDrawColor(ren, 255,255,255,255);
	SDL_RenderClear(ren);

	SDL_FPoint pa={100,100};
	SDL_FPoint pb={400,100};
	SDL_FPoint pc={250,100+150*SDL_sqrt(3)};
	snow(pb,pa,2);
	snow(pa,pc,2);
	snow(pc,pb,2);

	SDL_RenderPresent(ren);
	
	//unsigned int tick = 0;
    SDL_bool looping = SDL_TRUE;
	while(looping){
		
		SDL_Delay(20);
		//tick++;
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

