#include <stdlib.h>
#include <time.h>

#include "player.h"
#include "random.h"

#define Inf   0x10000000
#define NaN (-0x70000000)

int minmax_player::Evaluate(position * p, int color) {
    static const int val[8][8]={
        {512, 4,128,256,256,128, 4,512},
        {  4, 2,  8, 16, 16,  8, 2,  4},
        {128, 8, 64, 32, 32, 64, 8,128},
        {256,16, 32,  2,  2, 32,16,256},
        {256,16, 32,  2,  2, 32,16,256},
        {128, 8, 64, 32, 32, 64, 8,128},
        {  4, 2,  8, 16, 16,  8, 2,  4},
        {512, 4,128,256,256,128, 4,512}};
    int sum=0;
    for(int x=0;x<8;x++)
        for(int y=0;y<8;y++)
            sum+=p->GetField(x,y)*val[x][y];
    return color*sum;
}

bool minmax_player::MakeMove(position * p, int color, int *x,int *y) {
	int best[8][8];
	int bestval=-Inf;
	int ix, iy;
	for (ix=0; ix<8; ix++)
		for (iy=0; iy<8; iy++) {
			best[ix][iy]=NaN;
			prgrss(prgrss_target, (ix*8.0+iy)/64.0);
			if(!p->ValidMove(ix,iy,color)) continue;
            		position temp=*p;
            		temp.MakeMove(ix,iy,color);
			int btmp = best[ix][iy] = -MMEval(&temp,depth,enemy(color));
			if (btmp>bestval) bestval=btmp;
		}
	int bestcount=0;
	int target = bestval-100;
	for (ix=0; ix<8; ix++)
		for (iy=0; iy<8; iy++)
			if(best[ix][iy]>=target)
				bestcount++;
	int chosen = 1+random(bestcount);
	for (ix=0; (ix<8) && chosen ; ix++)
		for (iy=0; (iy<8) && chosen; iy++)
			if(best[ix][iy]>=target) {
				--chosen;
				*x=ix; *y=iy;
			}
	return true;
}

int minmax_player::MMEval(position * p,int level,int color) {
	if (level<=1)
		return Evaluate(p,color);
	if (!p->CanMove(color)) {
		if(p->CanMove(enemy(color)))
			return -MMEval(p,level-1,enemy(color));
		int i,e;
		p->GetScore(color==RED?&i:&e,color==BLUE?&i:&e);
		if(i>=e) return Inf;
		else return -Inf;
	}
	int bestval=-Inf;
	for (int ix=0; ix<8; ix++)
		for (int iy=0; iy<8; iy++) {
			if (!p->ValidMove(ix, iy, color)) continue;
            		position temp=*p;
            		temp.MakeMove(ix,iy,color);
			int btmp = -MMEval(&temp,level-1,enemy(color));
			if (btmp>bestval) bestval=btmp;
        	}
    return bestval;
}

