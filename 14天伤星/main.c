// 2022/2/24

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
	for(int curx=x-r;curx<=x+r;curx++){
		int dy=SDL_floor(0.5 + SDL_sqrt( (r+curx-x) * (r-curx+x) ) );
		SDL_RenderDrawLine(ren,curx,y+dy,curx,y-dy);
	}
}

typedef struct{
	Uint16 x,y;
	char ltr;
} LETTER;

void newletter(LETTER* pletter){
	pletter->y = 0;
	pletter->x = (Uint16)SDL_floor(0.5 + rand()/(float)RAND_MAX*400 );//随机出现
	pletter->ltr ='A'+ (char)SDL_floor(0.5 + rand()/(float)RAND_MAX*('Z'-'A') ) ;
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

	if(TTF_Init())puts("ttf error");
	TTF_Font *font;
	SDL_Surface* text=NULL;
	font=TTF_OpenFont("./font.ttf",30);
	if(!font)font=TTF_OpenFont("./font.ttc",30);
	if(!font){
		printf("Error to open font: %s\n",TTF_GetError());
		TTF_Quit();
		quitall();
		return 1;
	}
	TTF_SetFontStyle(font,TTF_STYLE_NORMAL);
	
	LETTER letter[3];
	srand((unsigned int)SDL_GetTicks());

	newletter(letter);
	newletter(letter+1);
	newletter(letter+2);
	Uint32 tick=0;
	SDL_bool looping=SDL_TRUE;
	int failtimes=0, suctimes=0;
	char drawltr[2];
	char ntcword[30];
	while(looping){
		SDL_SetRenderDrawColor(ren,0,0,0,255);
		SDL_RenderClear(ren);
		
		for(char i=0;i<3;i++){
			*drawltr=letter[i].ltr;
			DrawText(winSurface, font, drawltr, letter[i].x, i[letter].y);
			letter[i].y+=1;
			letter[i].y+=suctimes/10;
			if(letter[i].y>=470){
				looping = SDL_FALSE;
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "", "Game over!", win);
				break;
			}
		}
		char line=0;
		sprintf(ntcword, "\u65f6\u95f4%d",tick/50);
		DrawText(winSurface, font, ntcword, 450, 30*line++);
		sprintf(ntcword, "\u6b63\u786e%d",suctimes);
		DrawText(winSurface, font, ntcword, 450, 30*line++);
		sprintf(ntcword, "\u9519\u8bef%d",failtimes);
		DrawText(winSurface, font, ntcword, 450, 30*line++);
		sprintf(ntcword, "\u6b63\u786e\u7387%.1f%%",(suctimes||failtimes)?100*suctimes/(float)(suctimes+failtimes):100);
		DrawText(winSurface, font, ntcword, 450, 30*line++);
		sprintf(ntcword, "\u6bcf\u5206\u949f%d",tick?50*60*suctimes/tick:0);
		DrawText(winSurface, font, ntcword, 450, 30*line++);
		
        SDL_RenderPresent(ren);
		SDL_Delay(20);
		tick++;
		while(SDL_PollEvent(&ev)){
			switch(ev.type){
				case SDL_QUIT:
					looping = SDL_FALSE;
					break;
				case SDL_KEYDOWN:{
					SDL_bool trueflag = SDL_FALSE;
					for(char i=0;i<3;i++){
						if(ev.key.keysym.scancode == 
								SDL_SCANCODE_A + (letter[i].ltr - 'A') ){
							trueflag = SDL_TRUE;
							newletter(letter+i);
							break;
						}
					}
					if(trueflag)suctimes++;
					else failtimes++;
					break;
				}
			}
		}
	}


	TTF_CloseFont(font);
	TTF_Quit();
	SDL_FreeSurface(text);
	

	/// end drawing

//	SDL_RenderPresent(ren);
//	SDL_UpdateWindowSurface(win);

	quitall();

	return 0;
}

