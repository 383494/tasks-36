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

typedef struct Ballstr{
	int x;
	int y;
	int v;
	int angle;
	int r,g,b;
} Ball;

void updateball(Ball* bp, SDL_Rect rect){
	SDL_SetRenderDrawColor(ren, bp->r, bp->g, bp->b, 255);
	fillcircle(bp->x,bp->y,20);
	bp->x+=(int)SDL_floor(0.5+ bp->v*SDL_cos(bp->angle*PI/180.0));
	bp->y+=(int)SDL_floor(0.5+ bp->v*SDL_sin(bp->angle*PI/180.0));
	if((bp->x-25<=rect.x||bp->x+25>=rect.x+rect.w)&&
			(bp->y+25>=rect.y+rect.h||bp->y-25<=rect.y)){
		bp->angle+=180;
		bp->angle%=360;
	}
	else if(bp->x-25<=rect.x||bp->x+25>=rect.x+rect.w//检测碰撞
		){
		bp->angle=180-bp->angle;
		bp->angle%=360;
	}else if(bp->y+25>=rect.y+rect.h||bp->y-25<=rect.y){
		bp->angle=-bp->angle;
		bp->angle%=360;
	}
	
}

void initball(Ball* bp){
	bp->x = rand()/(double)RAND_MAX*550+50;
	bp->y = rand()/(double)RAND_MAX*400+50;
	bp->v = 10;
	bp->angle = 45+90*(rand()%4);
	bp->r = rand()/(double)RAND_MAX*255;
	bp->g = rand()/(double)RAND_MAX*225;
	bp->b = rand()/(double)RAND_MAX*225;
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

	SDL_Rect rect={0,0,640,480};

    struct Ballstr *balls;
	int ballnum=10;
	balls = (struct Ballstr*)SDL_calloc(10, sizeof(struct Ballstr));
	for(int i=0;i<ballnum;i++){
		initball(balls+i);
	}

	//unsigned int tick = 0;
    SDL_bool looping = SDL_TRUE;
	while(looping){
		SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderClear(ren);
		for(int i=0;i<ballnum;i++){
			updateball(balls+i, rect);
		}
		SDL_RenderPresent(ren);

		SDL_Delay(20);
		//tick++;
		while(SDL_PollEvent(&ev)){
			switch(ev.type){
				case SDL_QUIT:
					looping = SDL_FALSE;
					break;
				case SDL_KEYDOWN:
					switch(ev.key.keysym.scancode){
						case SDL_SCANCODE_EQUALS:
							ballnum++;
							balls=(struct Ballstr*)SDL_realloc(balls,ballnum*sizeof(struct Ballstr));
							initball(balls+ballnum-1);
							break;
						case SDL_SCANCODE_MINUS:
							if(ballnum==0)break;
							ballnum--;
							balls=(struct Ballstr*)SDL_realloc(balls,ballnum*sizeof(struct Ballstr));
							break;
					}
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

