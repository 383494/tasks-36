/// 2022/4/13
/// 贴图作者：@canned
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_image.h>
#include<stdio.h>
#include<stdbool.h>

#undef main
#undef MIN
#undef MAX
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define getdx(pos) (((pos)%2)*((pos)/2*2-1))	//0w1a2s3d
#define getdy(pos) ((((pos)+1)%2)*(((pos)+1)/2*2-1))

SDL_Window* win = NULL;
SDL_Surface* winSurface = NULL;
SDL_Renderer* ren = NULL;
SDL_Texture* tex = NULL;
SDL_Event ev;

typedef enum {
	BLOCK_AIR = 0
	, BLOCK_WALL
	, BLOCK_BOX
	, BLOCK_STONE//能推，但没什么用
	, BLOCK_REDSTONE
	, BLOCK_PISTON
	, BLOCK_PISTON_EXT//伸出的活塞
	, BLOCK_PISTON_ARM//活塞臂
	, BLOCK_TOTAL
} blocktype;

typedef struct{
	blocktype type;
	int data;//data^0x3为方向
			//data^0x4 红石：充能
} block;

const int SCRWID = 10;//*50
const int SCRHEI = 10;//*50
const int LEFTLEN = 10;//px
const int TOPLEN = 10;
const int FONTSIZE = 30;//字号
const int TEXTLEN = 5*FONTSIZE;//右侧提示文字长度
const blocktype rsblocks[] = {BLOCK_PISTON};//可被激活方块
const blocktype constblocks[] = {BLOCK_WALL
	, BLOCK_PISTON_ARM, BLOCK_PISTON_EXT};//不可推动方块
const blocktype tspblocks[] = {BLOCK_AIR};//透明方块

block** map = NULL;
blocktype **goal = NULL; //BLOCK_AIR==未指定
int mapsizex = 0, mapsizey = 0;
int goalAlpha = 255/3; //绘制目标时的透明度

SDL_Texture* pic[BLOCK_TOTAL] = { NULL };
SDL_Texture* playerpic[5] = {}, *errorpic = NULL;
TTF_Font *font = NULL;

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

void DrawText(TTF_Font* font, const char* text, int x, int y, int r, int g, int b)
{
	SDL_Color color = { r, g, b };
	SDL_Surface* textSfc = NULL;
	textSfc = TTF_RenderUTF8_Solid(font, text, color);
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

unsigned int playerx = 0, playery = 0, playerpos = 4;
unsigned int changetick = 0;
unsigned int camerax = 0, cameray = 0;
unsigned int leftGoals = 0;//剩余目标数
unsigned int stepCount = 0;//步数
unsigned int tick = 0;
bool looping = true;

bool playermove(int pos);//dx*dy==0,dx+dy!=0
bool pushbox(int x, int y, int dx, int dy);
bool isInList(blocktype item, const blocktype itemlist[]);
void updateBlock(int x, int y);
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

	win = SDL_CreateWindow(u8"推箱子游戏"
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
		printf("Error to init img: %s", IMG_GetError());
		quitall();
		return 1;
	}

	if (TTF_Init() != 0){
		printf("Error to init font: %s", TTF_GetError());
		IMG_Quit();
		quitall();
		return 1;
	}
	SDL_Surface* text=NULL;
	font = TTF_OpenFont("./font", FONTSIZE);
	if(!font){
		printf("Error to open font: %s",TTF_GetError());
		TTF_Quit();
		IMG_Quit();
		quitall();
		return 1;
	}
	TTF_SetFontStyle(font,TTF_STYLE_NORMAL);

	// 加载图片
	char filename[17];
	for (int i = 0; i < BLOCK_TOTAL; i++) {
		sprintf(filename, "img/%d.png", i);
		pic[i] = IMG_LoadTexture(ren, filename);
		SDL_SetTextureBlendMode(pic[i], SDL_BLENDMODE_BLEND);
	}
	for (int i = 0; i < 4;) {
		sprintf(filename, "img/player%d.png", i);
		playerpic[++i] = IMG_LoadTexture(ren, filename);
	}
	*playerpic = IMG_LoadTexture(ren, "img/player.png");

	SDL_SetTextureBlendMode(*playerpic, SDL_BLENDMODE_BLEND);
	errorpic = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 2, 2);
	SDL_SetRenderTarget(ren, errorpic);
	SDL_SetRenderDrawColor(ren, 163, 73, 164, 255);
	SDL_RenderDrawPoint(ren, 0, 0);
	SDL_RenderDrawPoint(ren, 1, 1);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderDrawPoint(ren, 0, 1);
	SDL_RenderDrawPoint(ren, 1, 0);
	SDL_SetRenderTarget(ren, NULL);

	loadmap();
	camerax = MAX(MIN(playerx, mapsizex-SCRWID/2), SCRWID/2) - SCRWID/2;
	cameray = MAX(MIN(playery, mapsizey-SCRHEI/2), SCRHEI/2) - SCRHEI/2;

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);

	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderDrawLine(ren, SCRWID*50+2*LEFTLEN, 0, SCRWID*50+2*LEFTLEN, SCRHEI*50+2*TOPLEN);
	drawscr();
	while (looping) {
		SDL_Delay(20);
		tick++;
		if(changetick == tick){
			SDL_Rect dst = {(playerx-camerax)*50+LEFTLEN, 
					(playery-cameray)*50+TOPLEN
					, 50, 50};
			SDL_RenderCopy(ren, *playerpic, NULL, &dst);
			SDL_RenderPresent(ren);
		}
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				looping = false;
				break;

			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					playermove(0);
					break;
				case SDL_SCANCODE_A:
					playermove(1);
					break;
				case SDL_SCANCODE_S:
					playermove(2);
					break;
				case SDL_SCANCODE_D:
					playermove(3);
					break;
				case SDL_SCANCODE_E:
					goalAlpha+=15;
					goalAlpha%=255;
					drawscr();
					break;
				case SDL_SCANCODE_R:
					goalAlpha-=15;
					goalAlpha%=255;
					drawscr();
					break;
				default:
					break;
				}
				break;
			default:break;
			}
		}
	}

	for(int x = 0; x < mapsizex; x++){
		free(map[x]);
		free(goal[x]);
	}
	free(map);
	free(goal);

	for (int i = 0; i < BLOCK_TOTAL; i++) {
		if (pic[i] != NULL)
			SDL_DestroyTexture(pic[i]);
	}

	for(int i = 0; i < BLOCK_TOTAL; i++){
		if(pic[i] != NULL)SDL_DestroyTexture(pic[i]);
	}
	if(*playerpic != NULL)
		SDL_DestroyTexture(*playerpic);
	SDL_DestroyTexture(errorpic);

	TTF_Quit();
	IMG_Quit();
	quitall();
	return 0;
}

void setRectPos(SDL_Rect* rect, int xn, int yn){
	rect->x = 50 * (xn - camerax) + LEFTLEN;
	rect->y = 50 * (yn - cameray) + TOPLEN;
}

bool playermove(int pos) {
	int dx = getdx(pos),dy = getdy(pos);
	playerpos = pos;
	changetick = tick+50;
	if(!pushbox(playerx, playery, dx, dy))return false;
	
	playerx += dx, playery += dy;
	if (playerx < 0 || playerx >= mapsizex ||
		 playery < 0 || playery >= mapsizey){

		playerx -= dx, playery -= dy;
		return false;
	}
	if (map[playerx][playery].type != BLOCK_AIR){
		//如果你被非透明方块封住
		if(playery>0 && map[playerx][playery-1].type == BLOCK_AIR)playery--;
		else if(playerx>0 && map[playerx-1][playery].type == BLOCK_AIR)playerx--;
		else if(playery<mapsizey-1 && map[playerx][playery+1].type == BLOCK_AIR)playery++;
		else if(playerx<mapsizex-1 && map[playerx+1][playery].type == BLOCK_AIR)playerx++;
	}

	camerax = MAX(MIN(playerx, mapsizex-SCRWID/2), SCRWID/2) - SCRWID/2;
	cameray = MAX(MIN(playery, mapsizey-SCRHEI/2), SCRHEI/2) - SCRHEI/2;

	stepCount++;
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
			if (map[cx][cy].type == BLOCK_AIR) {
				cx += dx, cy += dy;
				break;
			}
			if (isInList(map[cx][cy].type, constblocks))return false;
		}

		int tmpx=cx, tmpy=cy;
		//填充数组
		while (true) {
			cx -= dx, cy -= dy;
			if (cx == x && cy == y)break;
		
			if(goal[cx][cy] != BLOCK_AIR){
				if(goal[cx][cy] == map[cx-dx][cy-dy].type &&
					goal[cx][cy] != map[cx][cy].type)leftGoals--;
				else if(goal[cx][cy] == map[cx][cy].type &&
					goal[cx][cy] != map[cx-dx][cy-dy].type)leftGoals++;
				
				if(leftGoals == 0){
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
						u8"恭喜你", u8"你赢了", win);
					looping = false;
				}
			}
			map[cx][cy] = map[cx-dx][cy-dy];
		}
		cx=tmpx, cy=tmpy;
			
		while (true) {
			cx -= dx, cy -= dy;
			if (cx == x && cy == y)break;
			
			if(cx>0 && dx<=0)updateBlock(cx-1,cy);
			if(cy>0 && dy<=0)updateBlock(cx,cy-1);
			if(cx<mapsizex-1 && dx>=0)updateBlock(cx+1,cy);
			if(cy<mapsizey-1 && dy>=0)updateBlock(cx,cy+1);
		}

		map[cx][cy].type = BLOCK_AIR;
		map[cx][cy].data = 0;
	}
	return true;
}

bool isInList(blocktype item, const blocktype itemlist[]){
	int i = 0;
	while(itemlist[i])if(itemlist[i++]==item)return true;
	return false;
}

void updateBlock(int x, int y){
		bool cgd = 
			(x!=0 && map[x-1][y].data&0x4) ||
			(y!=0 && map[x][y-1].data&0x4) ||
			(x!=mapsizex-1 && map[x+1][y].data&0x4) ||
			(y!=mapsizey-1 && map[x][y+1].data&0x4) ;
		//是否充能	
	//intf("%d%d,%d,%d",x,y,cgd,map[x][y].type);puts("");	
		switch(map[x][y].type){
			case BLOCK_PISTON://不break
			case BLOCK_PISTON_EXT:
				int dx=getdx(map[x][y].data & 0x3), 
					dy=getdy(map[x][y].data & 0x3);
				if(map[x][y].type==BLOCK_PISTON && cgd &&
					x+dx>=0 && x+dx<=mapsizex-1 &&
					y+dy>=0 && y+dy<=mapsizey-1){
					if(pushbox(x, y, dx, dy)){
						map[x][y].type = BLOCK_PISTON_EXT;
						map[x][y].data = map[x+dx][y+dy].data;
						map[x+dx][y+dy].type = BLOCK_PISTON_ARM;
					}
				}
				else if(map[x][y].type==BLOCK_PISTON_EXT && !cgd){
					map[x][y].type = BLOCK_PISTON;
					map[x+dx][y+dy].type = BLOCK_AIR;
				}
			break;
			default:break;
		}
	
}

//绘制屏幕内容
void drawscr(){
	SDL_Rect dst = { LEFTLEN-1, TOPLEN-1, SCRWID*50+2, SCRHEI*50+2 };

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderFillRect(ren, &dst);
	dst.x = SCRWID*50+2*LEFTLEN+1;
	dst.y = TOPLEN;
	dst.w = TEXTLEN;
	dst.h = FONTSIZE*2;
	SDL_RenderFillRect(ren, &dst);

	char text[15];
	sprintf(text, u8"步数%d", stepCount);
	DrawText(font, text, SCRWID*50+2*LEFTLEN, TOPLEN, 0, 0, 0);
	sprintf(text, u8"剩余%d", leftGoals);
	DrawText(font, text, SCRWID*50+2*LEFTLEN, TOPLEN+FONTSIZE, 0, 0, 0);

	dst.w = 50, dst.h = 50;
	setRectPos(&dst, playerx, playery);
	SDL_RenderCopy(ren, playerpic[(playerpos+1)%5], NULL, &dst);

	unsigned int drawx = MIN(mapsizex, SCRWID), drawy = MIN(mapsizey, SCRHEI);

	SDL_Texture* srcpic = NULL;
	for (int x = camerax; x < camerax+drawx; x++) {
		for (int y = cameray; y < cameray+drawy; y++) {
			setRectPos(&dst, x, y);
			if(map[x][y].type < BLOCK_TOTAL)srcpic = pic[map[x][y].type];
			else srcpic = errorpic;

			SDL_RendererFlip flip = SDL_FLIP_NONE;
			double angle = 0;
			angle = -90 * (map[x][y].data & 0x3);
			
			SDL_RenderCopyEx(ren, srcpic, NULL, &dst, angle, NULL, flip);
			//if ((goal[x][y] != BLOCK_AIR) && (map[x][y].type == BLOCK_AIR)) {
				if(goal[x][y] < BLOCK_TOTAL)srcpic = pic[goal[x][y]];
				else srcpic = errorpic;
				SDL_SetTextureAlphaMod(srcpic, goalAlpha);
				SDL_RenderCopy(ren, srcpic, NULL, &dst);
				SDL_SetTextureAlphaMod(srcpic, 255);
			//}
		}
	}

	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	if(camerax == 0)SDL_RenderDrawLine(ren, LEFTLEN-1, TOPLEN-1, LEFTLEN-1, drawy*50+TOPLEN);
	if(cameray == 0)SDL_RenderDrawLine(ren, LEFTLEN-1, TOPLEN-1, drawx*50+LEFTLEN, TOPLEN-1);
	if(camerax+drawx == mapsizex)SDL_RenderDrawLine(ren, drawx*50+LEFTLEN, TOPLEN-1, drawx*50+LEFTLEN, drawy*50+TOPLEN);
	if(cameray+drawy == mapsizey)SDL_RenderDrawLine(ren, LEFTLEN-1, drawy*50+TOPLEN, drawx*50+LEFTLEN, drawy*50+TOPLEN);

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

	map = (block**)calloc(mapsizex, sizeof(void*));
	goal = (blocktype**)calloc(mapsizex, sizeof(void*));

	for (int i = 0; i < mapsizex; i++)map[i] = (block*)calloc(mapsizey, sizeof(block));
	for (int i = 0; i < mapsizex; i++)goal[i] = (blocktype*)calloc(mapsizey, sizeof(blocktype));

	if (fp == NULL)return;

	/// map
	for (int y = 0; y < mapsizey; y++) {
	for (int x = 0; x < mapsizex; x++) {
			fscanf(fp, "%d,", &map[x][y].type);
	}}

	fscanf(fp, "%d,%d", &playerx, &playery);
	if(playerx >= mapsizex)playerx = 0;
	if(playery >= mapsizex)playery = 0;

	while(fgetc(fp)!='\n');

	/// goal
	for (int y = 0; y < mapsizey; y++) {
	for (int x = 0; x < mapsizex; x++) {
			fscanf(fp, "%d,", &goal[x][y]);
			if((goal[x][y] != BLOCK_AIR) && 
				(goal[x][y] != map[x][y].type))leftGoals++;
	}}

	fseek(fp, 2L, SEEK_CUR);
	while(fgetc(fp)!='\n');

	/// datamap
	for (int y = 0; y < mapsizey; y++) {
	for (int x = 0; x < mapsizex; x++) {
			fscanf(fp, "%d,", &map[x][y].data);
			if(map[x][y].type == BLOCK_REDSTONE)map[x][y].data |= 0x4;//设置为充能状态
	}}

	fclose(fp);
}
