// 2022/2/21

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<stdio.h>
#undef main
#define PI 3.1415926535897932

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

void DrawText(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y, SDL_Renderer* ren, Uint8 R, Uint8 G, Uint8 B)
{
	SDL_Color textcolor={R,G,B};
	SDL_Surface* textSfc=NULL;
	textSfc=TTF_RenderText_Solid(font,text,textcolor);
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

    if(TTF_Init())puts("ttf init error");
    TTF_Font *font=TTF_OpenFont("./simsun.ttc",30);
	SDL_Surface* text=NULL;
	if(!font)printf("%s\n",TTF_GetError());
	TTF_SetFontStyle(font,TTF_STYLE_NORMAL);

    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderClear(ren);

    SDL_SetRenderDrawColor(ren,0,0,0,255);
    SDL_RenderDrawLine(ren,0,240,640,240);//x轴
    SDL_RenderDrawLine(ren,640,240,630,230);
    SDL_RenderDrawLine(ren,640,240,630,250);
    SDL_RenderDrawLine(ren,320,0,320,480);//y
    SDL_RenderDrawLine(ren,320,0,310,10);
    SDL_RenderDrawLine(ren,320,0,330,10);
	for(int i=1;i<=6;i++){//x number
		char j[3];
		sprintf(j,"%d",i);
		DrawText(winSurface,font,j,310+50*i,240,ren,0,0,0);
		i=-i;
		sprintf(j,"%d",i);
		DrawText(winSurface,font,j,310+50*i,240,ren,0,0,0);
		i=-i;
	}
	for(int i=-4;i<=4;i++){//y number
		char j[3];
		sprintf(j,"%d",i);
		DrawText(winSurface,font,j,320,240-50*i,ren,0,0,0);
		i=-i;
		sprintf(j,"%d",i);
		DrawText(winSurface,font,j,320,240-50*i,ren,0,0,0);
		i=-i;
	}
	SDL_SetRenderDrawColor(ren,255,0,0,255);
    for(int i=0;i<=640;i++){//sin x
        SDL_RenderDrawPoint(ren,i,(int) (240-50*SDL_sin((i-320)/50.0)));
    }
	SDL_SetRenderDrawColor(ren,0,255,0,255);
	for(float i=0;i<=30;i+=0.001){
		SDL_RenderDrawPointF(ren,320+10*i*SDL_cosf(i),240+10*i*SDL_sinf(i));
	}
	SDL_SetRenderDrawColor(ren,0,0,255,255);
	for(float i=0;i<=30;i+=0.001){
		SDL_RenderDrawPointF(ren,320+0.1*SDL_expf(i/PI)*SDL_cosf(i),240+0.1*SDL_expf(i/PI)*SDL_sinf(i));
	}




	SDL_RenderPresent(ren);

	while(SDL_WaitEvent(&ev)){
		if(ev.type == SDL_QUIT){
			break;
		}
	}

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(text);
	

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

