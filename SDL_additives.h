/*

  Ugly library of miscelaneous usually needed functions using SDL
  Composed by Wendigo

*/

#ifndef SDL_ADDITIVES_H
#define SDL_ADDITIVES_H

#include <SDL.h>

#define Delay 50

extern SDL_Surface *screen,*bak; // screen surface bitmap, backup bitmap (for ScrBackup & ScrRestore)

extern int bakX, bakY; // coordinates of bak surface

extern Uint8	BPP;
extern char	TURBO;
extern char	FULLSCREEN;
extern char	RESIZABLE;
extern unsigned int  WIDTH,HEIGHT;
extern char	IconF[255];
extern SDL_Surface *IconS;

extern SDL_Event event;
extern char Quit,Left,Right,Up,Down,Fire,Alt,Ctrl,Space,Setup,KeyDown;
extern Uint16 UC;

struct T_Mouse{
	int RX,RY,B,Block;
	int X;
	int Y;
	int ingame;
};

extern T_Mouse Mouse;

extern Uint32	LastTicks;

void RestCPU();
int E_FILTER(const SDL_Event *event);
void VIDEO_INIT(void);
void SDL_INIT_ALL(void);
void ClearSurfaceRGB(SDL_Surface *Co,char R, char G, char B);
SDL_Surface *NewSurface(int W,int H,Uint32 Fl);
SDL_Surface *GetSprite(SDL_Surface *From, int X, int Y, int W, int H);
void DrawSprite(SDL_Surface *Co,SDL_Surface *Kam,int X,int Y);
void DrawSpritePart(SDL_Surface *Co,SDL_Surface *Kam,int SX,int SY,int SW,int SH,int DX,int DY);
void ScrBackup(int X, int Y, int W, int H);
void ScrBackup();
void ScrRestore();

#endif
