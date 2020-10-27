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
#include "stdlib.h"
#include "string.h"
#include <SDL.h>
#include <SDL_ttf.h>
// Menu entrys
#define ME 7
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
void drawlinearity();		 // estado 4
void dropshadow();			 // estado 5
void stripedsprite();		 // estado 6
// general
void printSDLErrorAndReboot();
void go2XYprint(int x, int y, const char * text,unsigned char r,unsigned char g, unsigned char b);
SDL_Texture* loadTexture(const char * text, int linecode);
SDL_Texture* loadTextureA(const char * text, unsigned char r,unsigned char g, unsigned char b, int linecode);
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
// for linearity
SDL_Texture		*txtcirclesbmp = NULL;
SDL_Texture		*txtcircles_gridbmp = NULL;
SDL_Texture		*txtcircles_griddotbmp = NULL;
// for dropshadow
SDL_Texture		*txtmotokobmp = NULL;
SDL_Texture		*txtcheckposbmp = NULL;
SDL_Texture		*txtstripesposbmp = NULL;
SDL_Texture		*txtshadowbmp = NULL;
// for striped
SDL_Texture		*txtstripedbmp = NULL;

//  Window and font
int	W = 640, H = 480;
int FontH = 0;
// Menu
short slc = 0;
char Menu[ME][32] = {
	"Grid Test",
	"Color Bars",
	"Color Bars with Gray Scale",
	"Linearity",
	"Drop Shadow",
	"Striped Sprite",
	"Help"
};
// for linearity and drop shadow
unsigned char gType = 0; // 0 No grid, 1 Grid , 2 Grid dot
// for framecontrol dropshadow and others
unsigned char frame = 0;
SDL_Rect shadowdest = {320,240,64,64};

// controller
SDL_GameController *sgc;
// joy threshold
short JTHR = 50;
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
		printSDLErrorAndReboot(119);
    }
	
	
	if(TTF_Init() < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init: %s\n", TTF_GetError());
		SDL_VideoQuit();
		printSDLErrorAndReboot(126);
	}
	
	font = TTF_OpenFont("D:\\DejaVuSerif.ttf", 20);
	
	
	if(!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s\n", TTF_GetError());
		SDL_VideoQuit();
		printSDLErrorAndReboot(135);
	}
	
	FontH = TTF_FontHeight(font);
	
	if (SDL_NumJoysticks() <= 0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Joystick Error.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(143);
	}
	
	sgc = SDL_GameControllerOpen(0);
	
	if (sgc  == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Joystick Error.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(151);
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
		printSDLErrorAndReboot(168);
	}

	/* Create the renderer */
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer.\n");
		SDL_VideoQuit();
		printSDLErrorAndReboot(175);
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
			case 4:
				drawlinearity();
			break;
			case 5:
				dropshadow();
			break;
			case 6:
				stripedsprite();
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
		frame = !frame;
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
			printSDLErrorAndReboot(276);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtitlebmp = SDL_CreateTextureFromSurface(renderer, titlebmp);
		if (txtitlebmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(titlebmp);
			printSDLErrorAndReboot(284);
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
		txtgridbmp = loadTexture("D:\\grid.bmp",318);
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
			printSDLErrorAndReboot(358);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtcolorbmp = SDL_CreateTextureFromSurface(renderer, colorb);
		if (txtcolorbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(colorb);
			printSDLErrorAndReboot(366);
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
			printSDLErrorAndReboot(393);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txt601701cbbmp = SDL_CreateTextureFromSurface(renderer, d601color);
		if (txt601701cbbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(d601color);
			printSDLErrorAndReboot(401);
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

void drawlinearity(){
	//Render clear
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	
	// load textures
	if (txtcirclesbmp == NULL){
		// This only 
		SDL_Surface *circl = NULL;
		///////////////////////////////////////
		circl = SDL_LoadBMP("D:\\circles.bmp");
		if (circl == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(433);
		}
		/////////////////////////////////////// need alpha
		SDL_SetColorKey(circl, SDL_TRUE, SDL_MapRGB(circl->format, 0, 0, 0)); 
		
		///////////////////////////////////////
		/* Create textures from the image */
		txtcirclesbmp = SDL_CreateTextureFromSurface(renderer, circl);
		if (txtcirclesbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(circl);
			printSDLErrorAndReboot(444);
		}
		SDL_FreeSurface(circl);
	}
	
	if (txtcircles_gridbmp == NULL){
		// This only 
		SDL_Surface *gcircl = NULL;
		///////////////////////////////////////
		gcircl = SDL_LoadBMP("D:\\circles_grid.bmp");
		if (gcircl == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(456);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtcircles_gridbmp = SDL_CreateTextureFromSurface(renderer, gcircl);
		if (txtcircles_gridbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(gcircl);
			printSDLErrorAndReboot(464);
		}
		SDL_FreeSurface(gcircl);
	}
	
	if (txtcircles_griddotbmp == NULL){
		// This only 
		SDL_Surface *gdcircl = NULL;
		///////////////////////////////////////
		gdcircl = SDL_LoadBMP("D:\\circles_griddot.bmp");
		if (gdcircl == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
			printSDLErrorAndReboot(476);
		}
		///////////////////////////////////////
		/* Create textures from the image */
		txtcircles_griddotbmp = SDL_CreateTextureFromSurface(renderer, gdcircl);
		if (txtcircles_griddotbmp == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
			SDL_FreeSurface(gdcircl);
			printSDLErrorAndReboot(484);
		}
		SDL_FreeSurface(gdcircl);
	}
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	if(mctr.Y > 0){
		mctr.Y = 0;
		gType = gType < 2? gType + 1 : 0 ;
	}
	////////////////////////////////////
	// Textures
	switch(gType){
		case 1:
			SDL_RenderCopy(renderer, txtcircles_gridbmp, NULL, NULL);
		break;
		case 2:
			SDL_RenderCopy(renderer, txtcircles_griddotbmp, NULL, NULL);
		break;
	}
	SDL_RenderCopy(renderer, txtcirclesbmp, NULL, NULL);
	////////////////////////
	SDL_RenderPresent(renderer);
	// gamepad controller to change state
	if(mctr.B > 0){
		mctr.B = 0;
		estado = 0;
		gType  = 0;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void dropshadow(){
	SDL_Texture	*txtbg = NULL;
	short ndeltaX = 0, ndeltaY = 0;
	if (txtmotokobmp == NULL){
		txtmotokobmp = loadTexture("D:\\motoko.bmp",513);
	}
	if (txtcheckposbmp == NULL){
		txtcheckposbmp = loadTexture("D:\\checkpos.bmp",516);
	}
	if (txtstripesposbmp == NULL){
		txtstripesposbmp = loadTexture("D:\\stripespos.bmp",520);
	}
	if (txtshadowbmp == NULL){
		txtshadowbmp = loadTextureA("D:\\shadow.bmp",255,0,255,524);
	}
	
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	if(mctr.Y > 0){
		mctr.Y = 0;
		gType = gType < 2? gType + 1 : 0 ;
	}
	if(mctr.X > 0){
		mctr.X = 0;
		JTHR = JTHR > 10000 ? JTHR + 100 : 1000 ;
	}
	////////////////////////////////////
	// Textures
	switch(gType){
		case 0:
			txtbg = txtmotokobmp;
		break;
		case 1:
			txtbg = txtcheckposbmp;
		break;
		case 2:
			txtbg = txtstripesposbmp;
		break;
	}	
	
	SDL_RenderCopy(renderer, txtbg, NULL, NULL);
	if (frame){
		SDL_RenderCopy(renderer, txtshadowbmp, NULL, &shadowdest);
	}
	////////////////////////
	SDL_RenderPresent(renderer);
	/////////////////////////////////
	ndeltaX = SDL_GameControllerGetAxis(sgc,SDL_CONTROLLER_AXIS_RIGHTX);
	ndeltaY = SDL_GameControllerGetAxis(sgc,SDL_CONTROLLER_AXIS_RIGHTY);
	
	
	// move shadow next frame
	if (ndeltaX > 0){ // right
		
		if (ndeltaX > JTHR){
			shadowdest.x++;
		}
		
	}else if (ndeltaX < 0){ // left
		
		if (abs(ndeltaX) > JTHR){
			shadowdest.x--;
		}
	}


	if (ndeltaY > 0){ // down
		
		if (ndeltaY > JTHR){
			shadowdest.y++;
		}
	}else if (ndeltaY < 0){ // up
		
		if (abs(ndeltaY) > JTHR){
			shadowdest.y--;
		}
	}
	
	// gamepad controller to change state
	if(mctr.B > 0){
		mctr.B = 0;
		estado = 0;
		gType = 0;
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void stripedsprite(){
	SDL_Texture	*txtbg = NULL;
	short ndeltaX = 0, ndeltaY = 0;
	if (txtmotokobmp == NULL){
		txtmotokobmp = loadTexture("D:\\motoko.bmp",513);
	}
	if (txtcheckposbmp == NULL){
		txtcheckposbmp = loadTexture("D:\\checkpos.bmp",516);
	}
	if (txtstripesposbmp == NULL){
		txtstripesposbmp = loadTexture("D:\\stripespos.bmp",520);
	}
	if (txtstripedbmp == NULL){
		txtstripedbmp = loadTextureA("D:\\striped.bmp",255,0,255,524);
	}
	
	///////////////////////////
	///////////////////////////
	// gamepad controller in this state
	if(mctr.Y > 0){
		mctr.Y = 0;
		gType = gType < 2? gType + 1 : 0 ;
	}
	if(mctr.X > 0){
		mctr.X = 0;
		JTHR = JTHR > 10000 ? JTHR + 100 : 1000 ;
	}
	////////////////////////////////////
	// Textures
	switch(gType){
		case 0:
			txtbg = txtmotokobmp;
		break;
		case 1:
			txtbg = txtcheckposbmp;
		break;
		case 2:
			txtbg = txtstripesposbmp;
		break;
	}	
	
	SDL_RenderCopy(renderer, txtbg, NULL, NULL);
	if (frame){
		SDL_RenderCopy(renderer, txtstripedbmp, NULL, &shadowdest);
	}
	////////////////////////
	SDL_RenderPresent(renderer);
	/////////////////////////////////
	ndeltaX = SDL_GameControllerGetAxis(sgc,SDL_CONTROLLER_AXIS_RIGHTX);
	ndeltaY = SDL_GameControllerGetAxis(sgc,SDL_CONTROLLER_AXIS_RIGHTY);
	
	
	// move shadow next frame
	if (ndeltaX > 0){ // right
		
		if (ndeltaX > JTHR){
			shadowdest.x++;
		}
		
	}else if (ndeltaX < 0){ // left
		
		if (abs(ndeltaX) > JTHR){
			shadowdest.x--;
		}
	}


	if (ndeltaY > 0){ // down
		
		if (ndeltaY > JTHR){
			shadowdest.y++;
		}
	}else if (ndeltaY < 0){ // up
		
		if (abs(ndeltaY) > JTHR){
			shadowdest.y--;
		}
	}
	
	// gamepad controller to change state
	if(mctr.B > 0){
		mctr.B = 0;
		estado = 0;
		gType = 0;
	}
}



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
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

SDL_Texture* loadTexture(const char * text, int linecode){
	SDL_Texture *txt = NULL;
	// This only 
	SDL_Surface *sfbmp = NULL;
	///////////////////////////////////////
	sfbmp = SDL_LoadBMP(text);
	if (sfbmp == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
		printSDLErrorAndReboot(linecode);
	}
	///////////////////////////////////////
	/* Create textures from the image */
	txt = SDL_CreateTextureFromSurface(renderer, sfbmp);
	if (txt == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
		SDL_FreeSurface(sfbmp);
		printSDLErrorAndReboot(linecode);
	}
	SDL_FreeSurface(sfbmp);
	return txt;
}

SDL_Texture* loadTextureA(const char * text, unsigned char r,unsigned char g, unsigned char b, int linecode){
	SDL_Texture *txt = NULL;
	// This only 
	SDL_Surface *sfbmp = NULL;
	///////////////////////////////////////
	sfbmp = SDL_LoadBMP(text);
	if (sfbmp == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load texture.\n");
		printSDLErrorAndReboot(linecode);
	}
	///////////////////////////////////////
	SDL_SetColorKey(sfbmp, SDL_TRUE, SDL_MapRGB(sfbmp->format, r, g, b)); 
	///////////////////////////////////////////////
	/* Create textures from the image */
	txt = SDL_CreateTextureFromSurface(renderer, sfbmp);
	if (txt == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load Surface.\n");
		SDL_FreeSurface(sfbmp);
		printSDLErrorAndReboot(linecode);
	}
	SDL_FreeSurface(sfbmp);
	return txt;
}
