 /*
 * 480p Test Suite for ogxbox (Maybe)
 * Copyright (C) 2020 Leonardo Valdes Arteaga (eolandro)
 *
 * This file is based in 240p Test Suite by 2011 Artemio Urbina
 * 
 *
 * To Eleven is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The 240p Test Suite is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 480p Test Suite; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <hal/debug.h>
#include <hal/xbox.h>
#include <hal/video.h>
#include <windows.h>
#include "stdio.h"
#include "string.h"
#include <SDL.h>
#include <SDL_ttf.h>
// Menu entrys
#define ME 4

// La idea es realizar este programa como una maquina de estados
// The core of this software is a simple state machine

/*
 * Estado 0, Inicializando; State 0 initialize
 * Estado 99, Terminar; State 0 Exit
 */ 

// State
int estado = 0;
// functions
// states
void init();				 // estado 0
void grid();				 // estado 1
void colorbars();			 // estado 2
void draw601colorbars();	 // estado 3
// general
void printSDLErrorAndReboot();
void go2XYprint(int x, int y, const char * text,unsigned char r,unsigned char g, unsigned char b);
// SDL and screen things
const extern int SCREEN_WIDTH;
const extern int SCREEN_HEIGHT;
const char		*driver;
SDL_Window		*window;
SDL_Event		event;
SDL_Renderer	*renderer;
TTF_Font		*font;
SDL_Surface		*text_sf;
////////////////////////
// for title
SDL_Texture		*txtitlebmp = NULL;
// for grid		
SDL_Texture		*txtgridbmp = NULL;
// for colorbars	
SDL_Texture		*txtcolorbmp = NULL;
// for colorbars grey	
SDL_Texture		*txt601701cbbmp = NULL;

// test value
int test = 0;
//  Window and font
int	W = 640, H = 480;
int FontH = 0;
// Menu
short slc = 0;
char Menu[ME][32] = {
	"Grid Test",
	"Color Bars",
	"Color Bars with Gray Scale",
	"Help"
};
// controller
struct CTRLER {
	unsigned char UP;
	unsigned char DOWN;
	unsigned char LEFT;
	unsigned char RIGHT;
	unsigned char A;
	unsigned char B;
	unsigned char X;
	unsigned char Y;
	
} mctr = {
	0,0,0,0,0,0,0,0
};

int main(void){
	
	XVideoSetMode(W, H, 32, REFRESH_DEFAULT);
	// SDL init
	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL video.\n");
		printSDLErrorAndReboot(110);
    }
	
	
	if(TTF_Init() < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init: %s\n", TTF_GetError());
		SDL_VideoQuit();
		printSDLErrorAndReboot(117);
	}
	
	font = TTF_OpenFont("D:\\DejaVuSerif.ttf", 20);
	
	
	if(!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s\n", TTF_GetError());
		SDL_VideoQuit();
		printSDLErrorAndReboot(126);
	}
	
	FontH = TTF_FontHeight(font);
	
	if (SDL_NumJoysticks() <= 0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Joystick Error.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(134);
	}
	
	SDL_GameController *sgc = SDL_GameControllerOpen(0);
	
	if (sgc  == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Joystick Error.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(142);
	}
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
	driver = SDL_GetCurrentVideoDriver();

	window = SDL_CreateWindow("Suite480p",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	
	if(window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created!.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(158);
	}

	/* Create the renderer */
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(166);
	}

	/* Clear the window*/
	SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
	SDL_RenderClear(renderer);
	
	while (estado != 99){
		/////////////////////////////////////////////
		switch(estado){
			case 0:
				init();
			break;
			case 1:
				grid();
			break;
			case 2:
				colorbars();
			break;
			case 3:
				draw601colorbars();
			break;
			default:
				estado = 0;
			break;
		}
		//////////////////////////////////////////
		// event
		while (SDL_PollEvent(&event) == 1) {
			switch (event.type) {
				case SDL_QUIT:
					estado = 99;
				break;
				case SDL_CONTROLLERBUTTONDOWN:
					// check if detects gamecontroller
					test = 1;
					if (event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK){
						estado = 99;
					}else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP){
						mctr.UP = 1;
					}else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN){
						mctr.DOWN = 1;
					}else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A){
						mctr.A = 1;
					}else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B){
						mctr.B = 1;
					}else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_X){
						mctr.X = 1;
					}else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_Y){
						mctr.Y = 1;
					}
				break;
				default:
					
				break;
			}
		}
		//////////////////////////////////////////
	}
	TTF_CloseFont(font);
	
	if (txtitlebmp != NULL){
		SDL_DestroyTexture(txtitlebmp);
	}
	if (txtgridbmp != NULL){
		SDL_DestroyTexture(txtgridbmp);
	}
	if (txtcolorbmp != NULL){
		SDL_DestroyTexture(txtcolorbmp);
	}
	if (txt601701cbbmp != NULL){
		SDL_DestroyTexture(txt601701cbbmp);
	}
	
	SDL_DestroyWindow(window);
	SDL_GameControllerClose(0);
	SDL_VideoQuit();
	debugPrint("Bye!");
	Sleep(5000);
	XReboot();
	//  never get here
	return 0;
}

void init(){
	int dx = 80, dy = 110;
	short i = 0; // cause clasic
	short scolor = 0;
	///////////////////////////
	///////////////////////////
	
	if (txtitlebmp == NULL){
		// This only 
		SDL_Surface *titlebmp = NULL;
		///////////////////////////////////////
		titlebmp = SDL_LoadBMP("D:\\title.bmp");
		if (titlebmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(264);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtitlebmp = SDL_CreateTextureFromSurface(renderer, titlebmp);
		if (txtitlebmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(titlebmp);
			printSDLErrorAndReboot(272);
		}
		SDL_FreeSurface(titlebmp);
	}
	
	SDL_RenderCopy(renderer, txtitlebmp, NULL, NULL);
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	if(mctr.UP > 0){
		mctr.UP = 0;
		slc = slc > 0 ? (slc -1) : (ME - 1);
	}
	if(mctr.DOWN > 0){
		mctr.DOWN = 0;
		slc++;
		slc = slc % ME;
	}
	for (;i<ME;i++){
		scolor = i == slc ? 0 : 255; 
		go2XYprint(dx,dy,Menu[i],255,scolor,scolor); dy += FontH;
	}
	////////////////////////
	SDL_RenderPresent(renderer);
	// gamepad controller to change state
	if(mctr.A > 0){
		mctr.A = 0;
		estado = slc + 1;
	}
}

void grid(){
	if (txtgridbmp == NULL){
		// This only 
		SDL_Surface *gridbmp = NULL;
		///////////////////////////////////////
		gridbmp = SDL_LoadBMP("D:\\grid.bmp");
		if (gridbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(302);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtgridbmp = SDL_CreateTextureFromSurface(renderer, gridbmp);
		if (txtgridbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(gridbmp);
			printSDLErrorAndReboot(310);
		}
		SDL_FreeSurface(gridbmp);
	}
	
	SDL_RenderCopy(renderer, txtgridbmp, NULL, NULL);
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	
	////////////////////////
	SDL_RenderPresent(renderer);
	// gamepad controller to change state
	if(mctr.B > 0){
		mctr.B = 0;
		estado = 0;
	}
}

void colorbars(){
	if (txtcolorbmp == NULL){
		// This only 
		SDL_Surface *colorb = NULL;
		///////////////////////////////////////
		colorb = SDL_LoadBMP("D:\\color.bmp");
		if (colorb == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(337);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtcolorbmp = SDL_CreateTextureFromSurface(renderer, colorb);
		if (txtgridbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(colorb);
			printSDLErrorAndReboot(345);
		}
		SDL_FreeSurface(colorb);
	}
	
	SDL_RenderCopy(renderer, txtcolorbmp, NULL, NULL);
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	
	////////////////////////
	SDL_RenderPresent(renderer);
	// gamepad controller to change state
	if(mctr.B > 0){
		mctr.B = 0;
		estado = 0;
	}
}

void draw601colorbars(){
	if (txt601701cbbmp == NULL){
		// This only 
		SDL_Surface *d601color = NULL;
		///////////////////////////////////////
		d601color = SDL_LoadBMP("D:\\601701cb.bmp");
		if (d601color == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(379);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txt601701cbbmp = SDL_CreateTextureFromSurface(renderer, d601color);
		if (txt601701cbbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(d601color);
			printSDLErrorAndReboot(380);
		}
		SDL_FreeSurface(d601color);
	}
	
	SDL_RenderCopy(renderer, txt601701cbbmp, NULL, NULL);
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	
	////////////////////////
	SDL_RenderPresent(renderer);
	// gamepad controller to change state
	if(mctr.B > 0){
		mctr.B = 0;
		estado = 0;
	}
}

static void printSDLErrorAndReboot(int line){
    debugPrint("SDL_Error: %s\n", SDL_GetError());
    debugPrint("Line: %d\n", line);
    debugPrint("Rebooting in 5 seconds.\n");
    Sleep(5000);
    XReboot();
}

void go2XYprint(int x, int y, const char * text,unsigned char r,unsigned char g, unsigned char b){
	SDL_Rect destrect;
	int w, h;
	SDL_Surface		*txtsf;
	SDL_Color color={r,g,b};
	txtsf = TTF_RenderText_Solid(font,text,color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, txtsf);
	SDL_FreeSurface(txtsf);
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	destrect.x = x;
	destrect.y = y;
	destrect.w = w;
	destrect.h = h;
	SDL_RenderCopy(renderer, texture, NULL, &destrect);
	SDL_DestroyTexture(texture);
}
