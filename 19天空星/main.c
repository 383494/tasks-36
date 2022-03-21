// 2022/2/26
// 2022/2/28补档（做完不小心删了然后重新做了一次）

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

int prand(double* P){
	int ret = 0; 
	double tmp = (double)rand()/(double)RAND_MAX;
	do{
		if(tmp<=P[ret])return ret;
		else tmp-=P[ret];
	}while(++ret);
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

	srand((unsigned int)SDL_GetTicks());

	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);

	// x’ = a * x + b * y + e
	// y’ = c * x + d * y + f
	// p = 概率
	// x、y 值范围：0 ~ 1
	double a[5] = { 0.35173,  0.35338, 0.5,    0.5154,   0.00364 };
	double b[5] = { 0.35537, -0.3537,  0,     -0.0018,   0 };
	double c[5] = {-0.35537,  0.35373, 0,      0.00157,  0 };
	double d[5] = { 0.35173,  0.35338, 0.5,    0.58795,  0.57832 };
	double e[5] = { 0.3545,   0.2879,  0.25,   0.2501,   0.5016 };
	double f[5] = { 0.5,      0.1528,  0.462,  0.1054,   0.0606 };
	double p[5] = { 0.1773,   0.38,    0.1773, 0.2091,   0.0563 };

	double x, y;
	x=(double)rand()/(double)RAND_MAX;
	y=(double)rand()/(double)RAND_MAX;
	int n=0;
	
	for(int i=0;i<50000;i++){
		n=prand(p);
		x=a[n]*x+b[n]*y+e[n];
		y=c[n]*x+d[n]*y+f[n];
		SDL_RenderDrawPoint(ren, 480*x, 480-480*y);
	}
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

