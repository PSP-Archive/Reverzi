#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspkernel.h>


#include "SDL_additives.h"
#include "soundmanager.h"
#include "SFont.h"
#include "random.h"
#include "player.h"
#include "position.h"

#define NPL 4

#ifdef UNIX
#define XSTRNG(x) STRNG(x)
#define STRNG(x) #x
#define THEMEDIR XSTRNG(PREFIXDATA) "/img/"
#else
#define THEMEDIR "img/"
#endif
#define TITLETXT "Reverzi v1.0"

class UI{
	public:
		int ActivePlayer;
		int LastX, LastY;
		int PlayerType[2];
		player* Players[2];
		SDL_Surface *Faces[2];
		SDL_Surface *Titles[2];
		player* PlayerPool[2][NPL];
		SDL_Surface *FacePool[NPL];
		SDL_Surface *FaceThinkPool[NPL];
		SDL_Surface *StatusBG,*MenuBG,*GameTitle;
		SDL_Surface *cursor;

		position Board;
		SDL_Surface *S_Bg,*S_St[2],*S_Lm,*S_Pm;
		
		UI();
		void PlayGame();
		void DrawMenu();
		void DrawCursor();
		void DisplayProgress(double value);
		void Menu();
		void ScoreInfo();
		void CommitMove(int x, int y);
		void Draw(SDL_Surface *To);
		void Draw(SDL_Surface *To, int X, int Y, bool hilite);
		~UI();
};

void display_progress(void * target, double value) {
	((UI *) target)->DisplayProgress(value);
	SDL_PollEvent(&event);
}


UI::UI(){
	int i;
	SDL_Surface *bmp;

	ActivePlayer=EMPTY;
	for(i=0;i<2;i++) {
		PlayerPool[i][0]=new human_player();
		PlayerPool[i][1]=new minmax_player(display_progress, this, 1);
		PlayerPool[i][2]=new minmax_player(display_progress, this, 4);
		PlayerPool[i][3]=new minmax_player(display_progress, this, 6);
	}
	Mouse.X=20+120;
	Mouse.Y=10+320;
	InitFont(IMG_Load(THEMEDIR "font.png"));
	bmp=IMG_Load(THEMEDIR "cursor.png");
	cursor=GetSprite(bmp,0,0,bmp->w,bmp->h);

	bmp=IMG_Load(THEMEDIR "players.png");

	for(i=0;i<NPL;i++){
		FacePool[i]=GetSprite(bmp,i*80,0,80,40);
		FaceThinkPool[i]=GetSprite(bmp,i*80,40,80,bmp->h-40);
	}
	SDL_FreeSurface(bmp);

	PlayerType[0]=0;
	PlayerType[1]=2;

	bmp=IMG_Load(THEMEDIR "statusbar.png");
	StatusBG=GetSprite(bmp,0,0,320,40);
	MenuBG=GetSprite(bmp,0,40,320,40);
	SDL_FreeSurface(bmp);
	
	bmp=IMG_Load(THEMEDIR "title.png");
	GameTitle=GetSprite(bmp,0,0,bmp->w,bmp->h);
	SDL_FreeSurface(bmp);

	bmp=IMG_Load(THEMEDIR "background.png");
	S_Bg=GetSprite(bmp,0,0,320,320);
	SDL_FreeSurface(bmp);

	bmp=IMG_Load(THEMEDIR "tiles.png");
	S_St[0]=GetSprite(bmp,0,0,40,40);
	S_St[1]=GetSprite(bmp,40,0,40,40);
	S_Pm=GetSprite(bmp,80,0,40,40);
	S_Lm=GetSprite(bmp,120,0,40,40);
	SDL_FreeSurface(bmp);

	LastX=-1;
	LastY=-1;
}

void UI::ScoreInfo() {
	char Txt[100];
	int red,blue;
	Board.GetScore(&red,&blue);
	sprintf(Txt, (ActivePlayer==BLUE)?" %02d  x [%02d]":"[%02d] x  %02d ", red, blue);
	//SDL_WM_SetCaption(Txt,Txt);

	// Status graphics
	DrawSprite(StatusBG,screen,0,320);
	if ((ActivePlayer==RED)||(ActivePlayer==BLUE))
		DrawSpritePart(MenuBG,screen,(ActivePlayer==BLUE)?240:0,0,80,40,(ActivePlayer==BLUE)?240:0,320);
	DrawSprite(FacePool[PlayerType[iRED]],screen,0,320);
	DrawSprite(FacePool[PlayerType[iBLUE]],screen,240,320);
	sprintf(Txt,"%02d",red); PutString(screen,100-TextWidth(Txt)/2,341-FontHeight()/2,Txt);
	sprintf(Txt,"%02d",blue); PutString(screen,220-TextWidth(Txt)/2,341-FontHeight()/2,Txt);
	DrawSprite(cursor,screen,Mouse.X,Mouse.Y);
	SDL_UpdateRect(screen,0,320,320,40);
}

void UI::PlayGame() {
	LastX=-1;
	LastY=-1;

	Board.Reset();
	ActivePlayer=BLUE;
	Quit=0;
	for(;;) {
    		ActivePlayer=enemy(ActivePlayer);
    		Draw(screen);
			SDL_UpdateRect(screen,0,0,320,320);
			ScoreInfo();
			RestCPU();
			SDL_PollEvent(&event);
			//DrawCursor(); //denis
			
			if(Quit) return;
    		if (!Board.CanMove(ActivePlayer))
        		if (Board.CanMove(enemy(ActivePlayer))) continue;
        		else {
				//game over
				int red,blue;
				char Txt[100];
				Board.GetScore(&red,&blue);
				ActivePlayer=(red>blue)?RED:(red<blue)?BLUE:-1;
				ScoreInfo();
				sprintf(Txt,"%s",(red>blue)?"1st WINS":(red<blue)?"2nd WINS":"DRAW");
				PutString(screen,160-TextWidth(Txt)/2,341-FontHeight()/2,Txt);
				SDL_UpdateRect(screen,0,0,0,0);
				Quit=0;
            			return;
        		}

			DrawSprite(S_St[color2index(ActivePlayer)],screen,160-20,320);
			SDL_UpdateRect(screen,160-20,320,40,40);

			int x,y;
			if (Players[color2index(ActivePlayer)]->MakeMove(&Board,ActivePlayer,&x,&y)) { // AI move
        		CommitMove(x,y);
			playChunk(1);
        		continue;
	
    		} else { // human input
	    		bool Played=false;
				int LX=-1, LY=-1;
				Uint32 ThinkAnimDelay=2000,TicksToChange=SDL_GetTicks()+ThinkAnimDelay;
				char FaceAnimY=0;

				Quit=0;
				Mouse.B=0;
				do {
					if (SDL_GetTicks()>=TicksToChange){ // Thinking face update
						TicksToChange=SDL_GetTicks()+ThinkAnimDelay;
						DrawSpritePart(MenuBG,screen,(ActivePlayer==BLUE)?240:0,0,80,40,(ActivePlayer==BLUE)?240:0,320);
						DrawSpritePart(FaceThinkPool[PlayerType[color2index(ActivePlayer)]],screen,0,FaceAnimY,80,40,(ActivePlayer==BLUE)?240:0,320);
						SDL_UpdateRect(screen,(ActivePlayer==BLUE)?240:0,320,80,40);
						FaceAnimY=(FaceAnimY+40)%FaceThinkPool[PlayerType[color2index(ActivePlayer)]]->h;
					}
					RestCPU();
					SDL_PollEvent(&event);
					//DrawCursor(); //denis
					if ((LX!=Mouse.X/40)||(LY!=Mouse.Y/40)){
						LX=Mouse.X/40;
						LY=Mouse.Y/40;
						Draw(screen,LX,LY,Board.ValidMove(LX,LY,ActivePlayer));
						//DrawCursor(); //denis
						//ScoreInfo(); //denis
						SDL_UpdateRect(screen,0,0,0,0);
					}
					if (Mouse.B && Board.ValidMove(LX,LY,ActivePlayer))  Played=true; 
					}while ((!Quit)&&(!Played));
				if (Quit) return;
				CommitMove(LX,LY);
				playChunk(1);
				continue;
    		}
	}
}

void UI::DisplayProgress(double value) {
	if (value<0.25) return;
	DrawSpritePart(MenuBG,screen,(ActivePlayer==BLUE)?240:0,0,80,40,(ActivePlayer==BLUE)?240:0,320);
	DrawSpritePart(FaceThinkPool[PlayerType[color2index(ActivePlayer)]],screen,0,(int(value*4-1)*40)%FaceThinkPool[PlayerType[color2index(ActivePlayer)]]->h,80,40,(ActivePlayer==BLUE)?240:0,320);
	SDL_UpdateRect(screen,(ActivePlayer==BLUE)?240:0,320,80,40);
}

void UI::CommitMove(int x, int y) {
	LastX=x;
	LastY=y;
    Board.MakeMove(x,y,ActivePlayer);
    Draw(screen);
    SDL_UpdateRect(screen,0,0,0,0);
}

void UI::Draw(SDL_Surface *To) {
	DrawSprite(S_Bg,To,0,0);
	if(LastX+1) DrawSprite(S_Lm,To,LastX*40,LastY*40);
	for(int x=0;x<8;x++)
		for(int y=0;y<8;y++)
			if(Board.GetField(x,y)!=EMPTY)
				DrawSprite(S_St[color2index(Board.GetField(x,y))],To,x*40,y*40);
	DrawSprite(cursor,To,Mouse.X,Mouse.Y);
}

void UI::Draw(SDL_Surface *To, int X, int Y, bool hilite) {
	Draw(To);
	if(hilite) DrawSprite(S_Pm,To,X*40,Y*40);
}

void UI::DrawCursor() {
    	Draw(screen);
	DrawMenu();
	SDL_SetAlpha(cursor,SDL_SRCALPHA,128);
	DrawSprite(cursor,screen,Mouse.X,Mouse.Y);
}


void UI::DrawMenu() {
	Players[iRED]=PlayerPool[iRED][PlayerType[iRED]];
	Players[iBLUE]=PlayerPool[iBLUE][PlayerType[iBLUE]];
	Faces[iRED]=FacePool[PlayerType[iRED]];
	Faces[iBLUE]=FacePool[PlayerType[iBLUE]];
	DrawSprite(MenuBG,screen,0,320);

	DrawSprite(S_St[iRED],screen,80,320);
	DrawSprite(S_St[iBLUE],screen,200,320);

	DrawSprite(Faces[iRED],screen,0,320);
	DrawSprite(Faces[iBLUE],screen,240,320);

	DrawSprite(cursor,screen,Mouse.X,Mouse.Y);
	SDL_UpdateRect(screen,0,320,320,40);
}

void UI::Menu() {
restart:
	int I;
	char Restart=0;
	Mouse.B=0;
	Quit=0;

	ScrBackup(0,0,320,320);

	// Title fade in
	for (I=1;I<=5;I++){
		ScrRestore();
		SDL_SetAlpha(GameTitle,SDL_SRCALPHA,I*51);
		DrawSprite(GameTitle,screen,160-GameTitle->w/2,0);
		SDL_UpdateRect(screen,160-GameTitle->w/2,0,GameTitle->w,GameTitle->h);
		SDL_Delay(250);
		RestCPU();
	}
	//SDL_WM_SetCaption(TITLETXT,TITLETXT);
	
	DrawMenu();
	while(!Quit) {
		RestCPU();
		SDL_PollEvent(&event);
		DrawCursor(); //denis
		if (Mouse.B){
			if (Mouse.Y>=320){
				if (Mouse.X<80){
					PlayerType[iRED]=(PlayerType[iRED]+1)%NPL;
				}else if (Mouse.X>240){
					PlayerType[iBLUE]=(PlayerType[iBLUE]+1)%NPL;
				}else if ((Mouse.X>120)&&(Mouse.X<200)){
					Restart=1;
					Quit=1;
				}
				DrawCursor();
				Mouse.B=0;
			}
		}
	}

	// Title fade out
	for (I=4;I>0;I--){
		ScrRestore();
		SDL_SetAlpha(GameTitle,SDL_SRCALPHA,I*51);
		DrawSprite(GameTitle,screen,160-GameTitle->w/2,0);
		SDL_UpdateRect(screen,160-GameTitle->w/2,0,GameTitle->w,GameTitle->h);
		SDL_Delay(250);
		RestCPU();
	}
	
	Draw(screen);
	SDL_UpdateRect(screen,0,0,0,0);

	if (Quit>1)
		return;
	if (Restart) {
		Mouse.ingame=40;
		Mouse.X=20+120;
		Mouse.Y=10+120;
		PlayGame();
		Mouse.ingame=0;
		Mouse.B=0;
		while(!Mouse.B&&!Quit) {RestCPU(); SDL_PollEvent(&event);
		//DrawCursor(); //denis

		}
		goto restart;
	}
}

UI::~UI(){
	//FIXME deallocate stuff (low priority)
}
PSP_HEAP_SIZE_KB(20480);
#ifndef PSP
int main( int argc, char *argv[] )
#else
extern "C" int SDL_main( int argc, char *argv[] )
#endif
{

	randomize();

	SDL_INIT_ALL();

	UI * bfu = new UI();
	playMusic("song1");
	bfu->Draw(screen);
	bfu->DrawMenu();

	SDL_UpdateRect(screen,0,0,0,0);

	bfu->Menu();

	delete bfu;

	SDL_Quit();
	return 0;
}


