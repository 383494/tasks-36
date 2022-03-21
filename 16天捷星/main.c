// 2022/2/25

#include<SDL2/SDL.h>
//#include<SDL2/SDL_ttf.h>
#include<stdio.h>
#undef main
#define PI 3.1415926535897932

#define BTNNUM 8

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

typedef SDL_Rect button ;

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
    SDL_bool looping = SDL_TRUE;

	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderClear(ren);

    int state=1;//状态，0线 2圆 4矩 偶数标记 奇数画图
    SDL_Surface* saveSurface = NULL;
    button btn[BTNNUM];
	for(int i=0;i<BTNNUM;i++){
   		btn[i].x=200,btn[i].y=30*i,btn[i].w=20,btn[i].h=20;
	}
    SDL_SetRenderDrawColor(ren, 175,175,175,255);
    SDL_RenderFillRects(ren, btn, BTNNUM);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderDrawLine(ren, btn[0].x,btn[0].y,btn[0].x+btn[0].w,btn[0].y+btn[0].h);
	drawcircle(btn[1].x+btn[1].w/2, btn[1].y+btn[1].h/2, btn[1].w/2);
	SDL_RenderDrawRect(ren,btn+2);

	SDL_SetRenderDrawColor(ren,255,0,0,255);
	SDL_RenderFillRect(ren,btn+3);
	SDL_SetRenderDrawColor(ren,0,255,0,255);
	SDL_RenderFillRect(ren,btn+4);
	SDL_SetRenderDrawColor(ren,0,0,255,255);
	SDL_RenderFillRect(ren,btn+5);

	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderDrawLine(ren,btn[6].x+btn[6].w/2,btn[6].y,btn[6].x+btn[6].w/2,btn[6].y+btn[6].h+5);

	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderDrawLine(ren,btn[7].x+btn[7].w/2,btn[7].y-5,btn[7].x+btn[7].w/2,btn[7].y+btn[7].h);


    SDL_Rect drawableRect={0,0,100,100};//绘图区
    SDL_Point firstPoint;//标记点
    SDL_RenderPresent(ren);
	while(looping){
		//tick++;
		SDL_Delay(20);
		while(SDL_PollEvent(&ev)){
			switch(ev.type){
				case SDL_QUIT:
					looping = SDL_FALSE;
					break;
				case SDL_MOUSEBUTTONDOWN:
                    //puts("mouse button down");
                    if(ev.button.button == SDL_BUTTON_LEFT)
                        if(insideRect(ev.button.x, ev.button.y, drawableRect)){
							if(state%2==0){
								firstPoint.x = ev.button.x;
								firstPoint.y = ev.button.y;
								state++;
							}else{
								switch(state){
									case 1://线
										SDL_RenderDrawLine(ren, firstPoint.x, firstPoint.y,
											ev.button.x, ev.button.y);
										break;
									case 3://圆
										drawcircle(firstPoint.x, firstPoint.y,
											(int)SDL_floor(0.5+ SDL_sqrt(
												SDL_pow(firstPoint.x-ev.button.x,2)+SDL_pow(firstPoint.y-ev.button.y,2))
											));
										break;
									case 5://矩形
										SDL_RenderDrawLine(ren, firstPoint.x, firstPoint.y, firstPoint.x, ev.button.y);
										SDL_RenderDrawLine(ren, firstPoint.x, firstPoint.y, ev.button.x, firstPoint.y);
										SDL_RenderDrawLine(ren, ev.button.x, ev.button.y, ev.button.x, firstPoint.y);
										SDL_RenderDrawLine(ren, ev.button.x, ev.button.y, firstPoint.x, ev.button.y);
										break;
								}
								state--;
								SDL_RenderPresent(ren);
							}
						}
                        else for(int i=0;i<BTNNUM;i++){
                            if(insideRect(ev.button.x, ev.button.y, btn[i])){
								if(i<=2)
	                                state = 2*i;
								else switch(i){
									case 3://r
										SDL_SetRenderDrawColor(ren,255,0,0,255);
										break;
									case 4://g
										SDL_SetRenderDrawColor(ren,0,255,0,255);
										break;
									case 5://b
										SDL_SetRenderDrawColor(ren,0,0,255,255);
										break;
									case 6://download
										if(saveSurface)SDL_FreeSurface(saveSurface);
										saveSurface = SDL_CreateRGBSurface(0,100,100,32,0,0,0,0);
										SDL_RenderReadPixels(ren, &drawableRect, 0, saveSurface->pixels, drawableRect.w*4);
										if(SDL_SaveBMP(saveSurface, "save.bmp"))
											printf("error to save bmp: %s",SDL_GetError());
										SDL_FreeSurface(saveSurface);
										saveSurface = NULL;
										break;
									case 7://upload
										if(saveSurface)SDL_FreeSurface(saveSurface);
										saveSurface = SDL_LoadBMP("save.bmp");
										if(!saveSurface){
											printf("error loading bmp: %s",SDL_GetError());
											break;
										}
										SDL_Texture *bmp = SDL_CreateTextureFromSurface(ren, saveSurface);
										SDL_FreeSurface(saveSurface);
										SDL_RenderCopy(ren,bmp,NULL,&drawableRect);
										SDL_DestroyTexture(bmp);
										SDL_RenderPresent(ren);
										//SDL_UpdateWindowSurface(win);
										break;
								}
                                //printf("%d\n",state);
                            }
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

