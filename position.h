//---------------------------------------------------------------------------
#ifndef positionH
#define positionH
//---------------------------------------------------------------------------
#define RED 1
#define BLUE -1
#define EMPTY 0
#define iRED 0
#define iBLUE 1

inline int enemy(int color) {
    return -color;
}

inline int color2index(int color) {
	return (1-color)/2;
}

class position {
        int field[8][8];
    public:
        position();
        int GetField(int x,int y) {return field[x][y];}
        bool ValidMove(int x,int y,int color);
        void MakeMove(int x,int y,int color);
        bool CanMove(int player);
        void Reset();
	void GetScore(int *red, int * blue);
};
#endif

