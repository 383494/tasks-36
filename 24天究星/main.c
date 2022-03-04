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

typedef struct command{
	int from, to;
	struct command* next;
} cmd;

cmd *cmds = NULL, *tmp = NULL;

void move(int from, int to){
	if(cmds == NULL)cmds = (cmd*)malloc(sizeof(cmd));
	if(tmp == NULL)tmp = cmds;
	else{
		while(tmp->next)tmp = tmp->next;
		tmp->next = (cmd*)malloc(sizeof(cmd)); 
		tmp = tmp->next;
	}
	tmp->next = NULL;
	tmp->from = from;
	tmp->to = to;
}

void tower(int layers, int from, int to, int mid){//使用cmds(不含）以后的
	if(layers == 1){
		move(from, to);
	}
	else{
		tower(layers-1,from,mid,to);
		move(from,to);
		tower(layers-1,mid,to,from);
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

	tower(8,1,2,3);

	int rectnum[3]={8,0,0};

	SDL_Rect rect[8];
	for(int i=0;i<8;i++){
		rect[i].x=170;
		rect[i].y=480-30*(8-i);
		rect[i].w=10+10*i;
		rect[i].h=20;
	}

	
	unsigned int tick = 0;
    SDL_bool looping = SDL_TRUE;
	while(looping){
		if((tick%15==0)&&cmds){
			SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
			SDL_RenderClear(ren);
			rectnum[cmds->from-1]--;
			rectnum[cmds->to-1]++;
			//printf("%d,%d,%d,%d,%d\t",rectnum[0],rectnum[1],rectnum[2],cmds->from,cmds->to);
			for(int i=0;i<8;i++){
				if(rect[i].x/170==cmds->from){//move
					rect[i].x=cmds->to*170;
					rect[i].y=480-30*rectnum[cmds->to-1];
					//printf("%d",i);
					break;
				}
			}
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			for(int i=0;i<8;i++){
				SDL_RenderFillRect(ren, rect+i);
				rect[i].w*=-1;
				SDL_RenderFillRect(ren, rect+i);
			}
			tmp=cmds;
			if(cmds->next==NULL)cmds=NULL;
			else cmds=cmds->next;
			free(tmp);

			SDL_RenderPresent(ren);
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

	if(cmds){
		while(cmds->next){
			tmp=cmds;
			cmds=cmds->next;
			free(tmp);
		}
		free(cmds);
	}
	

	/// end drawing

//	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);

	quitall();

	return 0;
}

