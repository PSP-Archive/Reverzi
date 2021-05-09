#ifndef randomH
#define randomH


inline int random(int n) {
    return rand()%n;
}
inline void randomize() {
    srand(time(NULL));
}

#endif
