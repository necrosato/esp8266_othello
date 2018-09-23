#ifndef __OTHELLO__HPP
#define __OTHELLO__HPP

#include <ESP8266WiFi.h>
#include "Print.h"

class Othello {
public:
    typedef enum {
        OPEN = -1,
        BLACK = 0,
        WHITE = 1
    } Player;

    Othello(int size, HardwareSerial &s);
    ~Othello();
    int getSize();
    int **getGrid();
    void setGrid(int i, int j, int p);
    bool hasTurn(int p);
    int checkWin();
    void printBoard();
    int makeTurn(int i, int j);
    void reset();

    int **grid;
    int numBlack;
    int numWhite;
    int current;
    HardwareSerial &Serial;
private:
    int size;
    void flip(int i, int j);
    bool canFlip(int p, int i, int j, int d);
    bool checkCanFlip(int p, int i, int j, int d);
    bool checkTurn(int p, int i, int j);
};

class OthelloServer {
public: 
    OthelloServer(Othello &g) : game(g) {};
private:
    Othello &game;
};
#endif
