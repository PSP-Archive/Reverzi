//---------------------------------------------------------------------------
#include <assert.h>

#include "position.h"

//---------------------------------------------------------------------------
position::position() {
    Reset();
}
//---------------------------------------------------------------------------
void position::Reset() {
    for(int x=0;x<8;x++)
        for(int y=0;y<8;y++)
            field[x][y]=EMPTY;
    field[3][3]=field[4][4]=RED;
    field[3][4]=field[4][3]=BLUE;
}
//---------------------------------------------------------------------------
static inline bool Valid(int i) {
    return (i<8)&&(i>=0);
}
static inline bool Valid(int x,int y) {
    return Valid(x)&&Valid(y);
}
bool position::ValidMove(int x,int y,int color) {
    if(!Valid(x,y)) return false;
    if(GetField(x,y)!=EMPTY) return false;
    for(int xdelta=-1;xdelta<=1;xdelta++)
        for(int ydelta=-1;ydelta<=1;ydelta++) {
            if((!xdelta)&&(!ydelta)) continue;
            int tempx=x+xdelta,tempy=y+ydelta;
            while(Valid(tempx,tempy)&&(GetField(tempx,tempy)==enemy(color))) {
                tempx+=xdelta;
                tempy+=ydelta;
            }
            if (Valid(tempx,tempy)&&
                (GetField(tempx,tempy)==color)&&
                ((tempx!=x+xdelta)||(tempy!=y+ydelta)))
                return true;
        }
    return false;
}
//---------------------------------------------------------------------------
void position::MakeMove(int x,int y,int color) {
    assert(ValidMove(x,y,color));
    for(int xdelta=-1;xdelta<=1;xdelta++)
        for(int ydelta=-1;ydelta<=1;ydelta++) {
            if((!xdelta)&&(!ydelta)) continue;
            int tempx=x+xdelta,tempy=y+ydelta;
            while(Valid(tempx,tempy)&&(GetField(tempx,tempy)==enemy(color))) {
                tempx+=xdelta;
                tempy+=ydelta;
            }
            if (Valid(tempx,tempy)&&
                (GetField(tempx,tempy)==color)&&
                ((tempx!=x+xdelta)||(tempy!=y+ydelta)))
            {
                while((tempx!=x)||(tempy!=y)) {
                    field[tempx][tempy]=color;
                    tempx-=xdelta;
                    tempy-=ydelta;
                }
            }
        }
    field[x][y]=color;
}
//---------------------------------------------------------------------------
bool position::CanMove(int player) {
    for(int x=0;x<8;x++)
        for(int y=0;y<8;y++)
            if (ValidMove(x,y,player)) return true;
    return false;
}
//---------------------------------------------------------------------------
void position::GetScore(int *red, int *blue) {
	*red=0;
	*blue=0;
	for(int x=0;x<8;x++)
		for(int y=0;y<8;y++)
			switch(GetField(x,y)) {
				case RED:
					++*red;
					break;
				case BLUE:
					++*blue;
					break;
			}
}
//---------------------------------------------------------------------------

