// 2022/3/15
#define _CRT_SECURE_NO_WARNINGS

#include<SDL2/SDL.h>
//#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_image.h>
#include<stdio.h>
#include<stdbool.h>
#pragma comment(lib,"SDL2_image.lib")
#undef main
#define PI 3.1415926535897932

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
	, BLOCK_TOTAL
};

int** map = NULL;
int** goal = NULL; //BLOCK_AIR==未指定
int mapsizex = 0, mapsizey = 0;

SDL_Texture* pic[BLOCK_TOTAL] = { NULL };
SDL_Texture* playerpic = NULL;

void drawpol(float x, float y, float r, int sides) {
	SDL_FPoint pts[2];
	for (int i = 0; i <= sides; i++) {
		pts[i % 2].x = x + r * SDL_cosf(PI * 2 * i / (float)sides - PI / 2.0);
		pts[i % 2].y = y + r * SDL_sinf(PI * 2 * i / (float)sides - PI / 2.0);
		SDL_RenderDrawLinesF(ren, pts, 2);
	}
}

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

void drawcircle(int x, int y, int r) {
	int ody = 0;//old dy
	int dy = 0;
	for (int curx = x - r + 1; curx <= x + r; curx++) {
		dy = SDL_floor(0.5 + SDL_sqrt((r + curx - x) * (r - curx + x)));
		SDL_RenderDrawLine(ren, curx, y + dy, curx - 1, y + ody);
		SDL_RenderDrawLine(ren, curx, y - dy, curx - 1, y - ody);
		ody = dy;
	}
}

void fillcircle(int x, int y, int r) {
	drawcircle(x, y, r);
	for (int curx = x - r; curx <= x + r; curx++) {
		int dy = SDL_floor(0.5 + SDL_sqrt((r + curx - x) * (r - curx + x)));
		SDL_RenderDrawLine(ren, curx, y + dy, curx, y - dy);
	}
}

bool insideRect(int x, int y, SDL_Rect rect) {
	return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

int px, py;

bool playermove(int dx, int dy);//dx*dy==0,dx+dy!=0

void loadmap();

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Error to init: %s", SDL_GetError());
		quitall();
		return 1;
	}

	win = SDL_CreateWindow(u8"title"
		, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
		, 500, 500, SDL_WINDOW_SHOWN);
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

	if (IMG_Init(IMG_INIT_JPG) == 0) {
		printf("Error to init img: %s", SDL_GetError());
		quitall();
		return 1;
	}

	//srand((unsigned int)SDL_GetTicks());
	SDL_Rect rect = { 0,0,50,50 };


	// 加载图片
	char filename[9];
	for (int i = 0; i < BLOCK_TOTAL; i++) {
		sprintf(filename, "%d.jpg", i);
		pic[i] = IMG_LoadTexture(ren, filename);
	}
	playerpic = IMG_LoadTexture(ren, "player.jpg");

	loadmap();

	//unsigned int tick = 0;

	playermove(0, 0);

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

	IMG_Quit();

	//	SDL_RenderPresent(ren);
	//	SDL_UpdateWindowSurface(win);

	quitall();

	return 0;
}

bool playermove(int dx, int dy) {//负责计算和绘制
	if (dx != 0 && dy != 0)return false;
	if (dx != 0 || dy != 0) {
		int cx = px;
		int cy = py;

		//计算
		while (true) {
			cx += dx, cy += dy;
			if (cx < 0 || cx>9 || cy < 0 || cy>9)break;
			if (map[cx][cy] == BLOCK_AIR) {
				cx += dx, cy += dy;
				break;
			}
			if (map[cx][cy] == BLOCK_WALL)return false;
		}
		while (true) {
			cx -= dx, cy -= dy;
			if (cx == px && cy == py)break;
			map[cx][cy] = map[cx - dx][cy - dy];
		}
		map[cx][cy] = BLOCK_AIR;
		px += dx, py += dy;
		if (px < 0 || px>9 || py < 0 || py>9)px -= dx, py -= dy;
	}
	SDL_Rect dst = { 0,0,50,50 };
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);

	dst.x = 50 * px, dst.y = 50 * py;
	SDL_RenderCopy(ren, playerpic, NULL, &dst);

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			dst.x = 50 * x, dst.y = 50 * y;
			SDL_RenderCopy(ren, pic[map[x][y]], NULL, &dst);
			if (goal[x][y] != BLOCK_AIR) {
				SDL_SetTextureBlendMode(pic[goal[x][y]], SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod(pic[goal[x][y]], 255 / 3);
				SDL_RenderCopy(ren, pic[goal[x][y]], NULL, &dst);
				SDL_SetTextureAlphaMod(pic[goal[x][y]], 255);
			}
		}
	}


	SDL_RenderPresent(ren);


	return true;
}

void loadmap() {
	FILE* fp;
	fp = fopen("map.txt", "r");
	if (fp == NULL) {
		mapsizex = 10;
		mapsizey = 10;
	}
	else fscanf(fp, "%d,%d", &mapsizex, &mapsizey);

	map = (int**)calloc(mapsizex, sizeof(void*));
	goal = (int**)calloc(mapsizex, sizeof(void*));

	for (int i = 0; i < mapsizex; i++)map[i] = (int*)calloc(mapsizey, sizeof(int));
	for (int i = 0; i < mapsizex; i++)goal[i] = (int*)calloc(mapsizey, sizeof(int));

	if (fp == NULL)return;
	for (int y = 0; y < mapsizey; y++) {
		for (int x = 0; x < mapsizex-1; x++) {
			fscanf(fp, "%d,", map[x] + y);
			printf("%d", map[x][y]);
		}
		fscanf(fp, "%d", map[mapsizex-1] + y);
		puts("");
	}

	fscanf(fp, "%d,%d", &px, &py);

	for (int y = 0; y < mapsizey; y++) {
		for (int x = 0; x < mapsizex-1; x++) {
			fscanf(fp, "%d,", goal[x] + y);
			printf("%d", goal[x][y]);
		}
		fscanf(fp, "%d", goal[mapsizex-1] + y);
		puts("");
	}

	fclose(fp);
}

