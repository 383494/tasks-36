// 2022/2/24

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

	//Uint32 tick=0;
	SDL_Rect rectl={0,20,300,460}, rectr={340,20,300,460}, rectd={0,20,640,460};
	short bx=320, by=240;
	float btheta=0;
	short lx=150,ly=240,rx=490,ry=240;
	SDL_bool looping=SDL_TRUE;
	char pv=10,bv=5;//速度
	char pr=20,br=5;//半径
	char lp=0,rp=0;//分数
	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderClear(ren);

	while(looping){

		SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&rectd);
		SDL_SetRenderDrawColor(ren,255,175,175,255);
		SDL_RenderFillRect(ren,&rectl);
		SDL_SetRenderDrawColor(ren,175,175,255,255);
		SDL_RenderFillRect(ren,&rectr);

		SDL_SetRenderDrawColor(ren,0,0,0,255);
		fillcircle(bx, by, br);
		SDL_SetRenderDrawColor(ren,255,0,0,255);
		fillcircle(lx, ly, pr);
		SDL_SetRenderDrawColor(ren,0,0,255,255);
		fillcircle(rx, ry, pr);

		bx += (short)SDL_floor(0.5+ bv* SDL_cos(btheta));
		by += (short)SDL_floor(0.5+ bv* SDL_sin(btheta));

		if(SDL_sqrt(SDL_pow(bx-lx,2)+SDL_pow(by-ly,2)) < pr+br){
			btheta=PI-btheta+2*SDL_atan((ly-by)/(float)(lx-bx));
			bx += (short)SDL_floor(0.5+ bv* SDL_cos(btheta));
			by += (short)SDL_floor(0.5+ bv* SDL_sin(btheta));
		}
		if(SDL_sqrt(SDL_pow(bx-rx,2)+SDL_pow(by-ry,2)) < pr+br){
			btheta=PI-btheta+2*SDL_atan((ry-by)/(float)(rx-bx));
			bx += (short)SDL_floor(0.5+ bv* SDL_cos(btheta));
			by += (short)SDL_floor(0.5+ bv* SDL_sin(btheta));
		}
		if(by-br<=rectd.y||by+br>rectd.y+rectd.h){
			btheta*=-1;
			bx += (short)SDL_floor(0.5+ bv* SDL_cos(btheta));
			by += (short)SDL_floor(0.5+ bv* SDL_sin(btheta));
		}
		if(bx-br<=rectd.x||bx+br>=rectd.x+rectd.w)
		{
			if(bx-br<=rectd.x){
				rp++;
				SDL_SetRenderDrawColor(ren,175,175,255,255);
				fillcircle(640-30*rp,10,10);
			}
			else {
				lp++;
				SDL_SetRenderDrawColor(ren,255,175,175,255);
				fillcircle(30*lp,10,10);
			}
			if(rp==3||lp==3){
				looping=SDL_FALSE;
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"",
					lp==3?"Red is the winner":"Blue is the winner",
					win);
			}
			bx=320, by=240, btheta=0;
			lx=150, ly=240, rx=490, ry=240;
		}

		
        SDL_RenderPresent(ren);
		SDL_Delay(20);
		//tick++;
		while(SDL_PollEvent(&ev)){
			switch(ev.type){
				case SDL_QUIT:
					looping = SDL_FALSE;
					break;
				case SDL_KEYDOWN:{
					switch(ev.key.keysym.scancode){
						case SDL_SCANCODE_W:
							ly-=pv;
							if(ly-pr<rectl.y)ly=rectl.y+pr;
							break;
						case SDL_SCANCODE_A:
							lx-=pv;
							if(lx-pr<rectl.x)lx=rectl.x+pr;
							break;
						case SDL_SCANCODE_S:
							ly+=pv;
							if(ly+pr>rectl.y+rectl.h)ly=rectl.y+rectl.h-pr;
							break;
						case SDL_SCANCODE_D:
							lx+=pv;
							if(lx+pr>rectl.x+rectl.w)lx=rectl.x+rectl.w-pr;
							break;
						case SDL_SCANCODE_UP:
							ry-=pv;
							if(ry-pr<rectr.y)ry=rectr.y+pr;
							break;
						case SDL_SCANCODE_LEFT:
							rx-=pv;
							if(rx-pr<rectr.x)rx=rectr.x+pr;
							break;
						case SDL_SCANCODE_DOWN:
							ry+=pv;
							if(ry+pr>rectr.y+rectr.h)ry=rectr.y+rectr.h-pr;
							break;
						case SDL_SCANCODE_RIGHT:
							rx+=pv;
							if(rx+pr>rectr.x+rectr.w)rx=rectr.x+rectr.w-pr;
							break;
					}
					break;
				}
			}
		}
	}

	/// end drawing

//	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);

	quitall();

	return 0;
}

