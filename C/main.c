//////////////////////////////////
/* //Mario//			||
A simple mario game		||
	Methods & Objects	||
	-struct velocity	||
	-updatePosition		||
	-doGravity			||
	-doDirection		||
	-onGround			||
	-snapToGrid			||
	-doScrolling		||
						||
Version: Dev 0.8		||
//End//*///				||
//////////////////////////////////			


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
#include "Functions/mfunctions.h"
#include "Functions/Prototypes/mprototypes.h"
#include "Definitions/mdefines.h"
#include "Definitions/mpath.h"
#include "Structs/mstructs.h"



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
	int usingjoystick = 0;	//if using joystick

	char throwaway;
	char fname[100];

	int joy_x = 0;		//joystick increment

	SDL_Renderer	*renderer = NULL; 	//renderer declaration
	SDL_Window 	*window;		//window declaration

	SDL_Rect	grid_rect[GRIDY][GRIDX];//grid rect
	SDL_Rect	mario_rect;		//mario character
	SDL_Surface	*image;			//
	SDL_Texture	*mario_r;		//
	SDL_Texture	*mario_l;		//
	SDL_Texture	*mario_rj;		//
	SDL_Texture	*mario_lj;		//
	SDL_Texture	*blockpng[20];		//
	SDL_Surface	*text_texture;		//
	TTF_Font	*font = NULL;		//
	SDL_Color	textColor = {0, 0, 0};	//

	SDL_Joystick	*joystick = NULL;	//

	Mix_Chunk	*goose = NULL;		//
	Mix_Music	*background = NULL;	//

	SDL_Event	sdl_event;
	SDL_Event	sdl_event_discard[1000];

	struct velocity mario_velocity;		//velocity struct

	struct goomba 	goombas[100];		//

	const Uint8	*keystates;		//

	FILE *fh;


	///////////////////////////////////////////////////////////////////////////////
	//INITIALIZING

	doInit();

	window = SDL_CreateWindow("Mario", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOWX, WINDOWY, 0); //creating window

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //creating renderer

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//ARGUMENTS

	if(argc > 1){

		if(strcmp(argv[0], "editor")){

			execv(PATH"/Assets/Levels/marioleveleditor", NULL);
			exit(100);

		}

	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//FONT AND MUSIC


	//nothing to do here	


	/////////////////////////////////////////////////////////////////////////
	//IMAGE

	
	//facing right
	image = IMG_Load(PATH"/Assets/Images/Mario_dir1.png");

	mario_r = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//facing left
	image = IMG_Load(PATH"/Assets/Images/Mario_dir-1.png");

	mario_l = SDL_CreateTextureFromSurface(renderer, image);
	
	SDL_FreeSurface(image);

	//jumping right
	image = IMG_Load(PATH"/Assets/Images/mario_1_j.png");

	mario_rj = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//jumping left
	image = IMG_Load(PATH"/Assets/Images/mario_-1_j.png");

	mario_lj = SDL_CreateTextureFromSurface(renderer, image);

	SDL_FreeSurface(image);

	//blocks

	//image = IMG_Load("./Images/missingno.png");

	loadBlocks(blockpng, renderer);

	//SDL_FreeSurface(image);



	////////////////////////////////////////////////////////////////////////
	//FILE

	if((fh = fopen(PATH"/Assets/Levels/level1.grid", "r")) == NULL){

		printf("Error opening file\n");
		exit(2);

	}

	loadGrid(gGrid, fh);


	/////////////////////////////////////////////////////////////////	
	//joystick stuff

	joystick = loadJoysticks(joystick);

	if(joystick != NULL){

	usingjoystick = 1;	
	SDL_JoystickEventState(SDL_ENABLE);

	}
	
	//////////////////////////////////////////////////////////////////
	//placement

	mario_rect.x = 340; //mario placement
	mario_rect.y = 500;
	mario_rect.w = MARIOX;
	mario_rect.h = MARIOY;

	dummy_rect.x = 0;   //dummy placement
	dummy_rect.y = 0;
	dummy_rect.w = 0;
	dummy_rect.h = 0;

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
		


		isMoving = 0; //reset keypress
	
		SDL_PollEvent(&sdl_event); //poll events

		switch(sdl_event.type){ //switch events

			case SDL_QUIT: //if window close
				quit = 1;
				break;

			/*case SDL_KEYDOWN: //if keydown
				if(!usingjoystick){

					switch(sdl_event.key.keysym.sym){

						case SDLK_SPACE: //if space
							if(!inAir){ //check if in air
								mario_velocity.ya += 22; //jump velocity
								inAir = 1;
							}
							break;
						
						case SDLK_RIGHT: //if right
							isMoving = 1; //keydown flag
							lDirection = 1;//head direction flag
							mario_velocity.xa = ((mario_velocity.xa + SIDEINC/((12*(inAir+1)))) > SIDEINC) ? SIDEINC : (mario_velocity.xa + SIDEINC/(12*(inAir+1))); //velocity set (/2 if in air)
							break;

						case SDLK_LEFT: //if left
							isMoving = 1; 
							lDirection = 0;
							mario_velocity.xa = ((mario_velocity.xa - SIDEINC/(12*(inAir+1))) < -SIDEINC) ? -SIDEINC : (mario_velocity.xa - SIDEINC/(12*(inAir+1)));
							break;

					}

				}
				

				break; */

		} //switch sdl type

		SDL_PeepEvents(sdl_event_discard, 1000, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYDOWN); //trashes events


		//joysticks

		if(usingjoystick){

			SDL_JoystickUpdate();

			if(SDL_JoystickGetButton(joystick, 0) == SDL_PRESSED && !inAir){
				inAir = 1;
				mario_velocity.ya += 22;
			}

			/////////////////////////////////////////////////////////////////////////////////////////////////
			//CALCULATIONS

			joy_x = (SDL_JoystickGetAxis(joystick, 0) / 5333);
			isMoving = ((joy_x == 0) ? 0 : 1);

			prevVel = mario_velocity.xa;

			if(!(joy_x < 2 && joy_x > -2)){
			
				mario_velocity.xa = (abs((mario_velocity.xa + joy_x/((12.0*(inAir*3+1))))) > abs(SIDEINC)) ? (SIDEINC*(joy_x/(abs(joy_x)))) : (mario_velocity.xa + joy_x/(12.0*(inAir*3+1))); //velocity set (/2 if in air)
			} else {
				if(!inAir && isMoving){
					mario_velocity.xa = joy_x;
				}
			}
		
		}

		if(!usingjoystick){

			keystates = SDL_GetKeyboardState(NULL);

			if(keystates[SDL_SCANCODE_D]){

				isMoving = 1; //keydown flag
					lDirection = 1;//head direction flag
					mario_velocity.xa = ((mario_velocity.xa + SIDEINC/((12*(inAir+1)))) > SIDEINC) ? SIDEINC : (mario_velocity.xa + SIDEINC/(12*(inAir+1))); //velocity set (/2 if in air)
							
			} else if(keystates[SDL_SCANCODE_A]){

				isMoving = 1; 
				lDirection = 0;
				mario_velocity.xa = ((mario_velocity.xa - SIDEINC/(12*(inAir+1))) < -SIDEINC) ? -SIDEINC : (mario_velocity.xa - SIDEINC/(12*(inAir+1)));

			}

			if(keystates[SDL_SCANCODE_SPACE]){

				if(!inAir){ //check if in air
					mario_velocity.ya += 22; //jump velocity
					inAir = 1;
				}

			}

		}
		
		//printf("Vel:%lf Dir:%d  dum:%d\n", mario_velocity.xa, mDirection, dummy_rect.x); //prints debug info

		doScrolling(&dummy_rect, &mario_rect, &mario_velocity, gGrid);
		DEBUG_PRINT(("dummy: %d\n", dummy_rect.x));

		if(joy_x > 0){
			lDirection = 1;
		} else if(joy_x < 0){
			lDirection = 0;
		}
		doDirection(&mario_velocity, &mDirection); //calculates direction
	
		if(inAir){ //if in air
			doGravity(&mario_velocity); //applying gravity
			DEBUG_PRINT(("Gravitying\n"));
		}

		if(!isMoving && mario_velocity.xa != 0 /*&& !inAir*/){ //friction application	
			mario_velocity.xa -= (SIDEINC/12)/(inAir+1)*(mDirection);
			//printf("Frictioning!\n");
		}
			
		DEBUG_PRINT(("PrevVel:%lf Vel:%lf Dir:%d X:%d joy_x:%lf inAir:%d\n", prevVel, mario_velocity.xa, mDirection, ((int)mario_rect.x), (joy_x/((12.0*(inAir*3+1)))), inAir)); //prints debug info 

		if(!isMoving && (mario_velocity.xa < .50 && mario_velocity.xa > -.50)){
			mario_velocity.xa = 0;
		}
		//printf("Vel:%lf Dir:%d\n", mario_velocity.xa, mDirection); //prints debug info

		updatePosition(&mario_velocity, &mario_rect, &dummy_rect, gGrid, inAir); //updating position of mario
		if(onGround(&mario_rect, &dummy_rect, gGrid)){

			if(mario_velocity.ya <= 0){
				inAir = 0;
			}

		} else {
			inAir = 1;
		}
		mario_velocity.ya = ((inAir) ? mario_velocity.ya : 0); //in air gravity cancellation

		//////////////////////////////////////////////////////////////////////////////////////
		//RENDERING

		SDL_SetRenderDrawColor(renderer, 70, 100, 225, 255); //white background
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

		if(inAir){

			if(lDirection > 0){
				SDL_RenderCopy(renderer, mario_rj, NULL, &mario_rect);
			} else {
				SDL_RenderCopy(renderer, mario_lj, NULL, &mario_rect);
			}

		} else {

			if(lDirection > 0){
				SDL_RenderCopy(renderer, mario_r, NULL, &mario_rect);
			} else {
				SDL_RenderCopy(renderer, mario_l, NULL, &mario_rect);
			}

		}

		SDL_RenderPresent(renderer); //presenting

		SDL_Delay(10); //delay by 5

	}//while

	SDL_DestroyRenderer(renderer); //destorying, closing
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();



	return 0;

}

