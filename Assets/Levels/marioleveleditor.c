//////////////////////////////////
/* //Mario Editor//		||
Created by Alex Todaro		||
A simple mario game		||
	Methods & Objects	||
	-struct velocity	||
	-updatePosition		||
	-doGravity		||
	-doDirection		||
	-onGround		||
	-snapToGrid		||
	-doScrolling		||
				||
Version: N/A			||
//End//*///			||
//////////////////////////////////			

#define SIDEINC 9.0

#define WINDOWX 1024
#define WINDOWY 720
#define GRIDX 500
#define GRIDY ((WINDOWY/28) + 1)


//#define DEBUG


#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) (x)
#endif

 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

struct velocity {	//struct definition
	double xa;
	double ya;
};

void updatePosition(struct velocity *modifier, SDL_Rect *position);	//prototypes
void doGravity(struct velocity *toGravity);
void doDirection(struct velocity *dirCheck, int *direction);
int onGround(SDL_Rect *position);
void snapToGrid(SDL_Rect *position);
void doScrolling(SDL_Rect *position, struct velocity *modifier);
void incrementDummy(SDL_Rect *dummy, int inc);

int gGrid[GRIDY][GRIDX];

int inAir = 0;			//if mario on ground/in air

SDL_Rect	dummy_rect;

int main(int argc, char **argv){

	int i, j, k;		//for loop use	
	int quit = 0;		//if quit
	int isMoving = 0;	//if arrow key(s) down
	int mDirection = 0;	//left/right
	int lDirection = 1;	//looking l/r
	double prevVel;		//placeholder vel
	int clickx, clicky;	//click coords
	int blockType = 0;	//block type
	int doToggle = 0;
	int tup = 1;

	char throwaway;

	char helpPage [20][100];
	char fileName[100];
	char filePath[100];
	char blockTypeString[100];

	int joy_x = 0;		//joystick increment

	SDL_Renderer	*renderer = NULL; 	//renderer declaration
	SDL_Window 	*window;		//window declaration

	SDL_Rect	grid_rect[GRIDY][GRIDX];//grid rect
	SDL_Rect	mario_rect;		//mario character
	SDL_Rect	text_rect[20];		//
	SDL_Rect	blockt_rect;		//
	SDL_Surface	*image;			//
	SDL_Surface	*text_message[20];	//
	SDL_Surface	*blockt_message;	//
	SDL_Texture	*mario_r;		//
	SDL_Texture	*mario_l;		//
	SDL_Texture	*mario_rj;		//
	SDL_Texture	*mario_lj;		//
	SDL_Texture	*blockpng[20];		//
	SDL_Texture	*text_texture[20];	//
	SDL_Texture	*blockt_texture;	//
	TTF_Font	*font = NULL;		//
	SDL_Color	textColor = {255, 255, 255};	//
	SDL_Color	blocktColor = {0, 0, 0};//

	SDL_Joystick	*joystick = NULL;	//

	Mix_Chunk	*goose = NULL;		//
	Mix_Music	*background = NULL;	//

	SDL_Event	sdl_event;
	SDL_Event	sdl_event_discard[1000];

	const Uint8 	*keystates;		//

	struct velocity mario_velocity;		//velocity struct

	FILE *fh;

	srand(time( NULL ));


	///////////////////////////////////////////////////////////////////////////////
	//INITIALIZING
	srand(time( NULL ));


	printf("Enter level to edit: ");
	scanf("%s", fileName);
	sprintf(filePath, "./levels/%s.grid", fileName);

	printf("Opening %s!\n", filePath);


	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0){		//initializing SDL components
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if(TTF_Init() < 0){								//initializing TTF
		printf("Couldn't initialize SDL ttf extension: %s\n", SDL_GetError());	
		exit(1);
	}

	if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1){			//initializing audio

		printf("couldn't open mixer\n");
		exit(1);
	}

	window = SDL_CreateWindow("Mario Level Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOWX, WINDOWY, 0); //creating window

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //creating renderer


	
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//FONT AND MUSIC


	font = TTF_OpenFont("/usr/share/fonts/msttcorefonts/cour.ttf", 20);
	if(font == NULL) {
		printf("Can't open font, SDL_error = %s; TTF_error = %s\n", SDL_GetError(), TTF_GetError());
	
		return -1;
	}
	

	////////////////////////////////////////////////////////////////////////
	//TEXT

	for(i=0;i<20;i++){
		sprintf(helpPage[i], NULL);
	}

	sprintf(helpPage[0], "Arrow Keys: Move map.");
	sprintf(helpPage[1], "Left Click: Create block.");
	sprintf(helpPage[2], "Right Click: Delete block.");
	sprintf(helpPage[3], "0-9: Change block type.");
	

	for(i=0;i<20;i++){
		text_message[i] = TTF_RenderText_Solid(font, helpPage[i], textColor);
	}

	
	/////////////////////////////////////////////////////////////////////////
	//IMAGE

	
	IMG_Init(IMG_INIT_PNG);
	//facing right
	image = IMG_Load("./Images/Mario_dir1.png");

	mario_r = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//facing left
	image = IMG_Load("./Images/Mario_dir-1.png");

	mario_l = SDL_CreateTextureFromSurface(renderer, image);
	
	SDL_FreeSurface(image);

	//jumping right
	image = IMG_Load("./Images/mario_1_j.png");

	mario_rj = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//jumping left
	image = IMG_Load("./Images/mario_-1_j.png");

	mario_lj = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//blocks

	image = IMG_Load("./Images/missingno.png");

	for(i=0;i<20;i++){

		blockpng[i] = SDL_CreateTextureFromSurface(renderer, image);

	}

	SDL_FreeSurface(image);

	//block0
	image = IMG_Load("./Images/block0.png");

	blockpng[0] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//block1
	image = IMG_Load("./Images/block1.png");

	blockpng[1] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//block2
	image = IMG_Load("./Images/block2.png");

	blockpng[2] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//block5
	image = IMG_Load("./Images/block5.png");

	blockpng[5] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//block6
	image = IMG_Load("./Images/block6.png");

	blockpng[6] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//block7
	image = IMG_Load("./Images/block7.png");

	blockpng[7] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//block8
	image = IMG_Load("./Images/block8.png");

	blockpng[8] = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	////////////////////////////////////////////////////////////////////////
	//FILE

	if((fh = fopen(filePath, "r")) == NULL){

		printf("Error opening file\n");
		exit(2);

	}

	printf("Opening file\n");

	for(i=0;i<GRIDY;i++){

		for(j=0;j<GRIDX;j++){

			fscanf(fh, "%d", &gGrid[i][j]);

		}

		while ((throwaway = fgetc(fh)) != '\n') {
		}
                

	}


	
	//////////////////////////////////////////////////////////////////
	//placement

	mario_rect.x = 340; //mario placement
	mario_rect.y = 500;
	mario_rect.w = 42;
	mario_rect.h = 56;

	dummy_rect.x = 0;   //dummy placement
	dummy_rect.y = 0;
	dummy_rect.w = 0;
	dummy_rect.h = 0;

	for(i=0;i<20;i++){
		text_rect[i].x = 0;
		text_rect[i].y = 0;
		text_rect[i].w = 0;
		text_rect[i].h = 0;
	}

	blockt_rect.x = 5;
	blockt_rect.y = 5;
	blockt_rect.w = 0;
	blockt_rect.h = 0;

	for(i=0;i<GRIDY;i++){

		for(j=0;j<GRIDX;j++){

			grid_rect[i][j].y = i*28;
			grid_rect[i][j].x = j*28;
			grid_rect[i][j].w = 28;
			grid_rect[i][j].h = 28;

		}

	}

	//////////////////////////////////////////////////////////////////
	//velocities

	mario_velocity.xa = 0; //velocity zeroes
	mario_velocity.ya = 0;


	//////////////////////////////////////////////////////////////////
	//while

	while(!quit){
		//printf("Looping\n");


		isMoving = 0; //reset keypress
	
		SDL_PollEvent(&sdl_event); //poll events

		switch(sdl_event.type){ //switch events

			case SDL_QUIT: //if window close
				quit = 1;
				break;

			case SDL_KEYDOWN: //if keydown
				switch(sdl_event.key.keysym.sym){

					/*case SDLK_SPACE: //if space
						if(!inAir){ //check if in air
							mario_velocity.ya += 20; //jump velocity
							inAir = 1;
						}
						break;*/
						
					case SDLK_RIGHT: //if right
						incrementDummy(&dummy_rect, 28);
						break;

					case SDLK_LEFT: //if left
						incrementDummy(&dummy_rect, -28);
						break;

					case SDLK_t:

						if(tup){				
							if(!doToggle){
								doToggle = 1;
							} else {
								doToggle = 0;
							}
							tup = 0;
						}

						break;

					case SDLK_1:

						blockType = 1;

						break;

					case SDLK_2:
						blockType = 2;
						break;

					case SDLK_3:
						blockType = 3;
						break;

					case SDLK_4:
						blockType = 4;
						break;

					case SDLK_5:
						blockType = 5;
						break;

					case SDLK_6:
						blockType = 6;
						break;

					case SDLK_7:
						blockType = 7;
						break;

					case SDLK_8:
						blockType = 8;
						break;

					case SDLK_9:
						blockType = 9;
						break;

					case SDLK_0:
						blockType = 0;
						break;

				}
				

				break; 

			case SDL_KEYUP:

				switch(sdl_event.key.keysym.sym){

					case SDLK_t:

						tup = 1;

					break;

				}

			break;

			case SDL_MOUSEBUTTONDOWN: //mouse button

				if(!doToggle){
				
					if(sdl_event.button.button == SDL_BUTTON_LEFT){
	
						clickx = (sdl_event.button.x + dummy_rect.x)/28;
						clicky = sdl_event.button.y/28;

						gGrid[clicky][clickx] = blockType;

					}
					if(sdl_event.button.button == SDL_BUTTON_RIGHT){

						clickx = (sdl_event.button.x + dummy_rect.x)/28;
						clicky = sdl_event.button.y/28;

						gGrid[clicky][clickx] = 0;

					}

				}

				break;

		} //switch sdl type

		SDL_PeepEvents(sdl_event_discard, 1000, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYDOWN); //trashes events


		if(doToggle){

			SDL_PumpEvents();

			SDL_GetMouseState(&clickx, &clicky);

			clickx += dummy_rect.x;

			gGrid[clicky/28][clickx/28] = blockType;

		}


		/////////////////////////////////////////////////////////////////////////////////////////////////
		/*/CALCULATIONS//UNUSED


		printf("Vel:%lf Dir:%d  dum:%d\n", mario_velocity.xa, mDirection, dummy_rect.x); //prints debug info*/

		doScrolling(&dummy_rect, &mario_velocity);
		DEBUG_PRINT(("dummy: %d\n", dummy_rect.x));


		sprintf(blockTypeString, "[%d]", blockType);
		

		//////////////////////////////////////////////////////////////////////////////////////
		//RENDERING

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //white background
		SDL_RenderClear(renderer);

		for(i=0;i<GRIDY;i++){

			for(j=0;j<GRIDX;j++){

				grid_rect[i][j].x = j*28 - dummy_rect.x;

			}

		}


		for(i=0;i<GRIDY;i++){

			for(j=0,k=0;j<GRIDX;j++,k++){

				SDL_RenderCopy(renderer, blockpng[gGrid[i][j]], NULL, &grid_rect[i][j]);

			}

		}

		keystates = SDL_GetKeyboardState( NULL );

		if(keystates[SDL_SCANCODE_H]){

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			i = 0;

			while(helpPage[i] != NULL && text_message[i] != NULL){

				int w, h;

				TTF_SizeText(font, helpPage[i], &w, &h);				

				text_rect[i].x = 10;
				text_rect[i].y = 10 + (h + 2)*i;
				text_rect[i].w = w;
				text_rect[i].h = h;

				text_texture[i] = SDL_CreateTextureFromSurface(renderer, text_message[i]);

				SDL_RenderCopy(renderer, text_texture[i], NULL, &text_rect[i]);

				i++;

			}

		} else {

			int w, h;

			TTF_SizeText(font, blockTypeString, &w, &h);

			blockt_rect.x = WINDOWX - w - 5;
			blockt_rect.w = w;
			blockt_rect.h = h;

			blockt_message = TTF_RenderText_Solid(font, blockTypeString, blocktColor);

			blockt_texture = SDL_CreateTextureFromSurface(renderer, blockt_message);

			SDL_RenderCopy(renderer, blockt_texture, NULL, &blockt_rect);

		}
		

		SDL_RenderPresent(renderer); //presenting

		SDL_Delay(5); //delay by 10

	}//while

	SDL_DestroyRenderer(renderer); //destorying, closing
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	fclose(fh);

	if((fh = fopen(filePath, "w")) == NULL){

		printf("Error opening file\n");
		exit(2);

	}

	for(i=0;i<GRIDY;i++){

		for(j=0;j<GRIDX;j++){

			fprintf(fh, "%d ", gGrid[i][j]);

		}
		
		fprintf(fh, "\n");

	}

	fclose(fh);



	return 0;

}

void updatePosition(struct velocity *modifier, SDL_Rect *position){ //update mario y
	
	SDL_Rect test;
	
	//printf("PosX1: %lf", position->x);
	//position->x += /*((position->x > -1*modifier->xa && position->x < (678-abs(modifier->xa))) ?*/ modifier->xa /*: 0)*/;
	DEBUG_PRINT(("modX:%lf PosX2: %lf\n", modifier->xa, position->x));
	position->y -= modifier->ya;

	test.x = position->x;
	test.y = position->y;

	if(onGround(&test) && modifier->ya <= 0 && inAir){
	
		snapToGrid(position);
		inAir = 0;
		DEBUG_PRINT(("Snapping!\n"));

	}
	//position->y = (((modifier->ya < 0) && (position->y - modifier->ya > )) ?  586 : (position->y - modifier->ya));
}

void doGravity(struct velocity *toGravity){ //gravity application
	toGravity->ya -= 1.0;
}

void doDirection(struct velocity *dirCheck, int *direction){ //check and update direction
	if(dirCheck->xa == 0){
		*direction = 0;
	} else {
		*direction = ((dirCheck->xa > 0) ? 1 : -1);
	}		
}

int onGround(SDL_Rect *position){

	int x1, x2;
	int y;
	x1 = ((((int)position->x) + dummy_rect.x)/28);
	x2 = ((((int)position->x)+ 42 + dummy_rect.x)/28);
	y = ((((int)position->y) + 56)/28);
	DEBUG_PRINT(("x:%d, y:%d coord:%d\n", x1, y, gGrid[y][x1]));

	return ((gGrid[y][x1]==1 || gGrid[y][x2]==1) ? 1 : 0);

}


void snapToGrid(SDL_Rect *position){

	int x;
	int y;
	x = ((((int)position->x) + 21 + dummy_rect.x)/28);
	y = ((((int)position->y) + 56)/28) - 2;
	//position->x = x*28;
	position->y = y*28;

}

void doScrolling(SDL_Rect *dummy, struct velocity *modifier){

	dummy->x += modifier->xa;

}

void incrementDummy(SDL_Rect *dummy, int inc){

	dummy->x += inc;

}



