// 2022/3/2

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

typedef struct{
	int x, y;
	int wall;
} cellstr;

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

	SDL_SetRenderDrawColor(ren, 255,255,255,255);
	SDL_RenderClear(ren);

	SDL_SetRenderDrawColor(ren, 0,0,0,255);
	for(int i=1;i<12;i++){
		SDL_RenderDrawLine(ren,40*i,40,40*i,440);
		SDL_RenderDrawLine(ren,40,40*i,440,40*i);
	}
	
	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderDrawLine(ren,40,40,40,80);//擦掉一条
	SDL_RenderDrawLine(ren,440,400,440,440);

	cellstr cell[10][10];
//	int wall[10][10];
	for(int i=0;i<10;i++){
	for(int j=0;j<10;j++){
		cell[i][j].wall=4;
		cell[i][j].x=i;
		cell[i][j].y=j;
	}}
	/*
	for(int i=0; i<10; i++){
		if(i==0 || i==9){
			cell[i][0].wall=2;
			cell[0][i].wall=2;
			cell[i][9].wall=2;
			cell[9][i].wall=2;
		}else{
			cell[i][0].wall=3;
			cell[0][i].wall=3;
			cell[i][9].wall=3;
			cell[9][i].wall=3;
		}
	}
	*/
	//cell[0][0].wall=3;
	//wall[0][0]=3;
	int curx, cury;
	int visited = 1;
	cellstr* vcell[100]={cell[0]};//visitedCell
	cellstr* rcell[100]={cell[0]};//road
	int road = 1;
	SDL_bool roadflag = SDL_TRUE;//false结束
	curx = 0, cury = 0;
	while(visited<100){
		
		while(
			(curx==0?1:cell[curx-1][cury].wall<4)	//左边去过
			&&(cury==0?1:cell[curx][cury-1].wall<4)	//上
			&&(curx==9?1:cell[curx+1][cury].wall<4)	//右
			&&(cury==9?1:cell[curx][cury+1].wall<4)	//下
			
			//cell[curx][cury].wall==0
			){
				int i=rand()%visited;
				curx=vcell[i]->x;
				cury=vcell[i]->y;
				//puts("daf");
		}
		if(roadflag)
			while((rcell[road-1]->x != cell[curx][cury].x) || (rcell[road-1]->y != cell[curx][cury].y) ){ road--; }
		//printf("%d",road);
		int pos,i;
		i=1;
		//puts("d");
		if(curx>0)if(cell[curx-1][cury].wall==4) { pos=1; }
		if(cury>0)if(cell[curx][cury-1].wall==4)if(rand()<=(RAND_MAX/i++)){ pos=0; }
		if(curx<9)if(cell[curx+1][cury].wall==4)if(rand()<=(RAND_MAX/i++)){ pos=3; }
		if(cury<9)if(cell[curx][cury+1].wall==4)if(rand()<=(RAND_MAX/i++)){ pos=2; }
		//printf("%d,%d;%d;",curx, cury, pos);
		cell[curx][cury].wall--;
		//SDL_RenderDrawLine(ren, 40+curx*40+40*(pos/2), 40+cury*40+40*(pos%2), 60+curx*40+20*((pos+1)%4<2?-1:1), 60+cury*40+20*((pos+1)%4<2?-1:1));
		int oldx, oldy;
		oldx=curx, oldy=cury;
		curx+= pos%2==0 ? 0 : (pos/2)*2-1;
		cury+= pos%2==1 ? 0 : (pos/2)*2-1;
		SDL_RenderDrawLine(ren, (oldx+curx)*20+40, (oldy+cury)*20+60, (oldx+curx)*20+80, (oldy+cury)*20+60);
		SDL_RenderDrawLine(ren, (oldx+curx)*20+60, (oldy+cury)*20+40, (oldx+curx)*20+60, (oldy+cury)*20+80);
		vcell[visited++]=cell[curx]+cury;
		if(roadflag)rcell[road++]=cell[curx]+cury;
		if(cell[curx][cury].x==9 && cell[curx][cury].y==9)roadflag=SDL_FALSE;
		//printf("%d,%d\t",curx, cury);
		//puts("e");
		//SDL_RenderDrawLine(ren, oldx+20, oldy+20, curx+20, cury+20);
		//SDL_RenderDrawLine(ren, )
		cell[curx][cury].wall--;
		//puts("");
		//printf("%d/%d",visited,road);
	}
	SDL_SetRenderDrawColor(ren, 255,0,0,255);
	SDL_RenderDrawLine(ren, rcell[0]->x*40+60, rcell[0]->y*40+60, 40, 60);
	SDL_RenderDrawLine(ren, rcell[road-1]->x*40+60, rcell[road-1]->y*40+60, 440, 420);
	while(--road){
		SDL_RenderDrawLine(ren, rcell[road-1]->x*40+60, rcell[road-1]->y*40+60, rcell[road]->x*40+60, rcell[road]->y*40+60);
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

