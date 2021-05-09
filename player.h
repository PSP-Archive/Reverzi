#ifndef playerH
#define playerH

#include "position.h"

typedef void (*progress_callback) (void * target, double progress);

class player {
    protected:
        player() {}
    public:
        virtual bool MakeMove(position * p,int color,int *x,int *y)=0;
        virtual ~player() {}
};

class human_player : public player {
    public:
        human_player(){}
        virtual bool MakeMove(position * p,int color,int *x,int *y) {
            return false;
        }
};

class minmax_player : public player {
        int depth;
        int MMEval(position * p,int level,int color);
	progress_callback prgrss;
	void * prgrss_target;
	int Evaluate(position * p, int color);
    public:
        minmax_player(progress_callback p, void * t, int usedepth):
		player(), prgrss(p), prgrss_target(t), depth(usedepth)
	{}
        virtual bool MakeMove(position * p,int color,int *x,int *y);
};

#endif
