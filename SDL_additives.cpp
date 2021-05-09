#include <stdlib.h>
#include "SDL_additives.h"
#include "soundmanager.h"

extern void initSound();
SDL_Surface *screen,*bak=0; // screen surface bitmap, backup bitmap (for ScrBackup & ScrRestore)

int bakX, bakY; // coordinates of bak surface

SDL_Event event;

Uint32	LastTicks=SDL_GetTicks();
Uint8	BPP=16;
char	TURBO=0;
char	FULLSCREEN=0;
char	RESIZABLE=0;
unsigned int  WIDTH=320,HEIGHT=360;
char	IconF[255]="";
SDL_Surface *IconS=0;

char Quit=0,Left=0,Right=0,Up=0,Down=0,Fire=0,Alt=0,Ctrl=0,Space=0,Setup=0,KeyDown=0;
Uint16 UC=0;
T_Mouse Mouse;


void RestCPU(){
	Uint32 ActTicks=SDL_GetTicks();
	if ((ActTicks-LastTicks)<Delay)
		SDL_Delay(Delay-(ActTicks-LastTicks));
	LastTicks=ActTicks;
}

int E_FILTER(const SDL_Event *event) {


	if ( event->type ==  SDL_JOYBUTTONDOWN ) {
		if (event->jbutton.button==2)
			Mouse.B=1;
		else if (event->jbutton.button==0)
			Mouse.B=2;
		else if (event->jbutton.button==10)
			Quit=2;
		else if (event->jbutton.button==7) {
			Mouse.X-=40;
			if (Mouse.X<0)
				Mouse.X+=40;
			playChunk(0);
		}
		else if (event->jbutton.button==9) {
			Mouse.X+=40;
			if (Mouse.X>320)
				Mouse.X-=40;
			playChunk(0);
		}
		else if (event->jbutton.button==8) {
			Mouse.Y-=40;
			if (Mouse.Y<0)
				Mouse.Y+=40;
			playChunk(0);
			
		}
		else if (event->jbutton.button==6) {
			Mouse.Y+=40;
			if (Mouse.Y>360-Mouse.ingame)
				Mouse.Y-=40;
			playChunk(0);
		}
		return 0;
	}

	if ( event->type == SDL_JOYBUTTONUP ) {
		Mouse.B=0;
		Mouse.Block=0;
		return 0;
	}


	if (event->type==SDL_QUIT) Quit=2;
	
	return(1);
}


void VIDEO_INIT(void){
	Uint32 videoflags=0;

	if (BPP==0)
		videoflags = SDL_ANYFORMAT;
	if (FULLSCREEN) videoflags=videoflags | SDL_FULLSCREEN;
	if (RESIZABLE) videoflags=videoflags | SDL_RESIZABLE;
	if (TURBO) videoflags=videoflags | SDL_HWSURFACE;
	else videoflags=videoflags | SDL_SWSURFACE;

	if ( (screen=SDL_SetVideoMode(WIDTH,HEIGHT,BPP,videoflags)) == NULL ) {
		fprintf(stderr, "Couldn't set %ix%i video mode: %s\n",WIDTH,HEIGHT,SDL_GetError());
		exit(2);
	}
}


void SDL_INIT_ALL(void){
	 SDL_Joystick *joy;
	Uint32 InitFlags;

	InitFlags=SDL_INIT_VIDEO|SDL_INIT_JOYSTICK;

	if ( SDL_Init(InitFlags) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);



	//SDL_ShowCursor(SDL_ENABLE);
	SDL_SetEventFilter(E_FILTER);
	SDL_EnableKeyRepeat(0,1);
	VIDEO_INIT();

	joy=SDL_JoystickOpen(0);
	
	initSound();

}


void ClearSurfaceRGB(SDL_Surface *Co,char R, char G, char B){
	SDL_Rect clip;

	clip.x = 0;
	clip.y = 0;
	clip.w = Co->w;
	clip.h = Co->h;

	SDL_FillRect (Co, &clip, SDL_MapRGB(Co->format,R,G,B));
}


SDL_Surface *NewSurface(int W,int H,Uint32 Fl){
	return SDL_CreateRGBSurface(Fl,W,H,BPP,screen->format->Rmask,screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
}

SDL_Surface *GetSprite(SDL_Surface *From, int X, int Y, int W, int H){
	SDL_Surface *P;
	SDL_Surface *To;
	SDL_Rect Src;
	
	if (!From){
		return 0;
	}else{
		P=NewSurface(W,H,SDL_HWSURFACE|SDL_SRCCOLORKEY);
		Src.x=X;
		Src.y=Y;
		Src.w=W;
		Src.h=H;
		SDL_BlitSurface(From,&Src,P,NULL);
		SDL_SetColorKey(P,SDL_SRCCOLORKEY|SDL_RLEACCEL,SDL_MapRGB(P->format,255,0,255));
		To=SDL_DisplayFormat(P);
		SDL_FreeSurface(P);
	}
	return To;
}

void DrawSprite(SDL_Surface *Co,SDL_Surface *Kam,int X,int Y){
SDL_Rect Dest;
	if (Co==NULL) return;
	Dest.x=X;
	Dest.y=Y;
	Dest.w=Co->w;
	Dest.h=Co->h;
	SDL_BlitSurface(Co,NULL,Kam,&Dest);
}

void DrawSpritePart(SDL_Surface *Co,SDL_Surface *Kam,int SX,int SY,int SW,int SH,int DX,int DY){
SDL_Rect Src,Dest;
	if (Co==NULL) return;
	Src.x=SX;
	Src.y=SY;
	Src.w=SW;
	Src.h=SH;
	Dest.x=DX;
	Dest.y=DY;
	Dest.w=SW;
	Dest.h=SH;
	SDL_BlitSurface(Co,&Src,Kam,&Dest);
}

void ScrBackup(int X, int Y, int W, int H){
	SDL_Rect Src;

	bakX=Src.x=X;
	bakY=Src.y=Y;
	Src.w=W;
	Src.h=H;

	if (bak)
		SDL_FreeSurface(bak);

	bak=NewSurface(W,H,SDL_HWSURFACE);
	SDL_BlitSurface(screen,&Src,bak,NULL);
}

void ScrBackup(){
	ScrBackup(0,0,WIDTH,HEIGHT);
}

void ScrRestore(){
	DrawSprite(bak,screen,bakX,bakY);
}


