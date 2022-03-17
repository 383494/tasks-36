// 2022/3/17
#define _CRT_SECURE_NO_WARNINGS

#include<SDL2/SDL.h>
//#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_image.h>
#include<stdio.h>
#include<stdbool.h>

#undef main
#undef MIN
#undef MAX
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

const int SCRWID = 10;//*50
const int SCRHEI = 10;//*50
const int LEFTLEN = 10;//px
const int TOPLEN = 10;
const int TEXTLEN = 100;//右侧提示文字长度

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

enum {
	BLOCK_AIR
	, BLOCK_WALL
	, BLOCK_BOX
	, BLOCK_STONE//能推，但没什么用
	, BLOCK_PISTON//todo
	, BLOCK_PISTON_ARM
	, BLOCK_TOTAL
};

int** map = NULL, **datamap = NULL, **goal = NULL; //BLOCK_AIR==未指定
int mapsizex = 0, mapsizey = 0;

SDL_Texture* pic[BLOCK_TOTAL] = { NULL };
SDL_Texture* playerpic = NULL, *errorpic = NULL;

void quitall() {
	if (winSurface)
		SDL_FreeSurface(winSurface);
	if (tex)
		SDL_DestroyTexture(tex);
	if (ren)
		SDL_DestroyRenderer(ren);
	if (win)
		SDL_DestroyWindow(win);
	SDL_Quit();
}

#ifdef SDL_TTF_H_
void DrawText(SDL_Surface* screen, TTF_Font* font, const char* text, int x, int y)
{
	SDL_Color white = { 255,255,255 };
	SDL_Surface* textSfc = NULL;
	textSfc = TTF_RenderUTF8_Solid(font, text, white);
	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(ren, textSfc);
	SDL_Rect dest = { 0, 0, textSfc->w, textSfc->h };
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = textSfc->w;
	rect.h = textSfc->h;
	SDL_FreeSurface(textSfc);
	SDL_RenderCopy(ren, text_texture, &dest, &rect);
	SDL_DestroyTexture(text_texture);
}
#endif

unsigned int playerx, playery;
unsigned int camerax = 0, cameray = 0;

bool playermove(int dx, int dy);//dx*dy==0,dx+dy!=0
bool pushbox(int x, int y, int dx, int dy);
void drawscr();

void setRectPos(SDL_Rect* rect, int xn, int yn);
void loadmap();

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Error to init: %s", SDL_GetError());
		quitall();
		return 1;
	}

	win = SDL_CreateWindow(u8"\u63a8\u7bb1\u5b50\u6e38\u620f"
		, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
		, 2*LEFTLEN + SCRWID*50 + TEXTLEN, 2*TOPLEN + SCRHEI*50
		, SDL_WINDOW_SHOWN);
	if (!win) {
		printf("Error to create window: %s", SDL_GetError());
		quitall();
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, 0);
	if (!ren) {
		printf("Error to create renderer: %s", SDL_GetError());
		quitall();
		return 1;
	}

	winSurface = SDL_GetWindowSurface(win);
	if (!winSurface) {
		printf("Error to get surface: %s", SDL_GetError());
		quitall();
		return 1;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0) {
		printf("Error to init img: %s", SDL_GetError());
		quitall();
		return 1;
	}

	// 加载图片
	char filename[15];
	for (int i = 0; i < BLOCK_TOTAL; i++) {
		sprintf(filename, "img/%d.png", i);
		pic[i] = IMG_LoadTexture(ren, filename);
		SDL_SetTextureBlendMode(pic[i], SDL_BLENDMODE_BLEND);
	}
	playerpic = IMG_LoadTexture(ren, "img/player.png");
	SDL_SetTextureBlendMode(playerpic, SDL_BLENDMODE_BLEND);
	errorpic = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 2, 2);
	SDL_SetRenderTarget(ren, errorpic);
	SDL_SetRenderDrawColor(ren, 163, 73, 164, 255);
	SDL_RenderDrawPoint(ren, 0, 0);
	SDL_RenderDrawPoint(ren, 1, 1);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderDrawPoint(ren, 0, 1);
	SDL_RenderDrawPoint(ren, 1, 0);
	//SDL_RenderPresent(ren);
	SDL_SetRenderTarget(ren, NULL);

	loadmap();
	camerax = MAX(MIN(playerx, mapsizex-SCRWID/2), SCRWID/2) - SCRWID/2;
	cameray = MAX(MIN(playery, mapsizey-SCRHEI/2), SCRHEI/2) - SCRHEI/2;

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);

	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderDrawLine(ren, SCRWID*50+2*LEFTLEN, 0, SCRWID*50+2*LEFTLEN, SCRHEI*50+2*TOPLEN);
	drawscr();

	SDL_bool looping = SDL_TRUE;
	while (looping) {
		SDL_Delay(20);
		//tick++;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				looping = SDL_FALSE;
				break;

			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					playermove(0, -1);
					break;
				case SDL_SCANCODE_A:
					playermove(-1, 0);
					break;
				case SDL_SCANCODE_S:
					playermove(0, 1);
					break;
				case SDL_SCANCODE_D:
					playermove(1, 0);
					break;
				default:break;
				}
				break;
			default:break;
			}
		}
	}

	for (int i = 0; i < BLOCK_TOTAL; i++) {
		if (pic[i] != NULL)
			SDL_DestroyTexture(pic[i]);
	}

	for(int x = 0; x < mapsizex; x++){
		free(map[x]);
		free(datamap[x]);
		free(goal[x]);
	}
	free(map);
	free(datamap);
	free(goal);

	for(int i = 0; i < BLOCK_TOTAL; i++){
		if(pic[i] != NULL)SDL_DestroyTexture(pic[i]);
	}
	if(playerpic != NULL)
		SDL_DestroyTexture(playerpic);
	SDL_DestroyTexture(errorpic);

	IMG_Quit();
	quitall();
	return 0;
}

void setRectPos(SDL_Rect* rect, int xn, int yn){
	rect->x = 50 * (xn - camerax) + LEFTLEN;
	rect->y = 50 * (yn - cameray) + TOPLEN;
}

bool playermove(int dx, int dy) {
	if(!pushbox(playerx, playery, dx, dy))return false;
	
	playerx += dx, playery += dy;
	if (playerx < 0 || playerx >= mapsizex || playery < 0 || playery >= mapsizey)playerx -= dx, playery -= dy;

	camerax = MAX(MIN(playerx, mapsizex-SCRWID/2), SCRWID/2) - SCRWID/2;
	cameray = MAX(MIN(playery, mapsizey-SCRHEI/2), SCRHEI/2) - SCRHEI/2;

	drawscr();

	return true;
}

bool pushbox(int x, int y, int dx, int dy){
	if (dx != 0 && dy != 0)return false;
	if (dx != 0 || dy != 0) {
		int cx=x, cy=y;
		//计算
		while (true) {
			cx += dx, cy += dy;
			if (cx < 0 || cx >= mapsizex || cy < 0 || cy >= mapsizey)break;
			if (map[cx][cy] == BLOCK_AIR) {
				cx += dx, cy += dy;
				break;
			}
			if (map[cx][cy] == BLOCK_WALL)return false;
		}
	
		//填充数组
		while (true) {
			cx -= dx, cy -= dy;
			if (cx == x && cy == y)break;
			map[cx][cy] = map[cx - dx][cy - dy];
		}
		map[cx][cy] = BLOCK_AIR;
	}
	return true;
}

void drawscr(){
	
	SDL_Rect dst = { LEFTLEN-1, TOPLEN-1, SCRWID*50+2, SCRHEI*50+2 };

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderFillRect(ren, &dst);

	dst.w = 50, dst.h = 50;
	setRectPos(&dst, playerx, playery);
	SDL_RenderCopy(ren, playerpic, NULL, &dst);

	SDL_Texture* srcpic = NULL;
	for (int x = camerax; x < camerax+10; x++) {
		for (int y = cameray; y < cameray+10; y++) {
			setRectPos(&dst, x, y);
			if(map[x][y] < BLOCK_TOTAL)srcpic = pic[map[x][y]];
			else srcpic = errorpic;
			SDL_RenderCopy(ren, srcpic, NULL, &dst);
			if (goal[x][y] != BLOCK_AIR) {
				if(goal[x][y] < BLOCK_TOTAL)srcpic = pic[goal[x][y]];
				else srcpic = errorpic;
				SDL_SetTextureAlphaMod(srcpic, 255 / 3);
				SDL_RenderCopy(ren, srcpic, NULL, &dst);
				SDL_SetTextureAlphaMod(srcpic, 255);
			}
		}
	}

	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	if(camerax == 0)SDL_RenderDrawLine(ren, LEFTLEN-1, TOPLEN-1, LEFTLEN-1, SCRHEI*50+TOPLEN);
	if(cameray == 0)SDL_RenderDrawLine(ren, LEFTLEN-1, TOPLEN-1, SCRWID*50+LEFTLEN, TOPLEN-1);
	if(camerax+SCRWID == mapsizex)SDL_RenderDrawLine(ren, SCRWID*50+LEFTLEN, TOPLEN-1, SCRWID*50+LEFTLEN, SCRHEI*50+TOPLEN);
	if(cameray+SCRHEI == mapsizey)SDL_RenderDrawLine(ren, LEFTLEN-1, SCRHEI*50+TOPLEN, SCRWID*50+LEFTLEN, SCRHEI*50+TOPLEN);

	SDL_RenderPresent(ren);
}

void loadmap() {
	FILE* fp;
	fp = fopen("map.txt", "r");
	if (fp == NULL) {
		mapsizex = 10;
		mapsizey = 10;
	}
	else fscanf(fp, "%d,%d", &mapsizex, &mapsizey);

	while(fgetc(fp)!='\n');

	map = (int**)calloc(mapsizex, sizeof(void*));
	datamap = (int**)calloc(mapsizex, sizeof(void*));
	goal = (int**)calloc(mapsizex, sizeof(void*));

	for (int i = 0; i < mapsizex; i++)map[i] = (int*)calloc(mapsizey, sizeof(int));
	for (int i = 0; i < mapsizex; i++)datamap[i] = (int*)calloc(mapsizey, sizeof(int));
	for (int i = 0; i < mapsizex; i++)goal[i] = (int*)calloc(mapsizey, sizeof(int));

	if (fp == NULL)return;
	for (int y = 0; y < mapsizey; y++) {
		for (int x = 0; x < mapsizex-1; x++) {
			fscanf(fp, "%d,", map[x] + y);
		}
		fscanf(fp, "%d", map[mapsizex-1] + y);
	}

	fscanf(fp, "%d,%d", &playerx, &playery);

	while(fgetc(fp)!='\n');

	for (int y = 0; y < mapsizey; y++) {
		for (int x = 0; x < mapsizex-1; x++) {
			fscanf(fp, "%d,", goal[x] + y);
		}
		fscanf(fp, "%d", goal[mapsizex-1] + y);
	}

	while(fgetc(fp)!='\n');

	fclose(fp);
}
