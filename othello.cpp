#include "othello.hpp"

Othello::Othello(int size, HardwareSerial &s) : Serial(s) {
    this->size = size;
    this->grid = new int *[size];
    this->reset();
}

Othello::~Othello() {
    for (int i = 0; i < this->size; i++)
        delete[] this->grid[i];
    delete[] this->grid;
}

int Othello::getSize() {
    return this->size;
}

int** Othello::getGrid() {
    return this->grid;
}

void Othello::setGrid(int i, int j, int p) {
    this->grid[i][j] = p;
}

bool Othello::hasTurn(int p) {
    for (int i = 0; i < this->size; i++)
        for (int j = 0; j < this->size; j++)
            for (int k = 0; k < 8; k++)
                if (this->grid[i][j] == OPEN)
                    if (checkCanFlip(p, i, j, k))
                        return true;
    return false;
}

int Othello::checkWin() {
    numWhite = 0, numBlack = 0;
    int winner = -2;
    for (int i = 0; i < this->size; i++)
        for (int j = 0; j < this->size; j++) {
            switch (this->grid[i][j]) {
                case OPEN: winner = OPEN; break;
                case BLACK: numBlack++; break;
                case WHITE: numWhite++; break;
            }
        }
    Serial.print("Player 1: ");
    Serial.print(numBlack);
    Serial.print(",      Player 2: ");
    Serial.println(numWhite);
    if (numBlack == 0)
        winner = WHITE;
    else if (numWhite == 0)
        winner = BLACK;
    
    if (winner == -1)
        return winner;
    else if (numBlack > numWhite)
        winner = BLACK;
    else if (numWhite > numBlack)
        winner = WHITE;
    Serial.print("Player ");
    Serial.print(winner + 1);
    Serial.print(" won!\n");
    return winner;
}

void Othello::printBoard() {
    Serial.print("  ");
    for (int i = 0; i < this->size; i++) {
        Serial.print(' ');
        Serial.print(i);
    }
    Serial.println();
    for (int i = 0; i < this->size; i++) {
        Serial.print(' ');
        Serial.print(i);
        for (int j = 0; j < this->size; j++) {
            if (this->grid[i][j] == BLACK) Serial.print(" b");
            else if (this->grid[i][j] == WHITE) Serial.print(" w");
            else Serial.print(" .");
        }
        Serial.println();
    }
}

void Othello::flip(int i, int j) {
    this->grid[i][j] = !(this->grid[i][j]);
}

bool Othello::canFlip(int p, int i, int j, int d) {
    int iOffset = 0, jOffset = 0, nextI, nextJ;
    switch (d) {
        case 0:
            iOffset = -1;
            jOffset = -1;
            break;
        case 1:
            iOffset = -1;
            break;
        case 2:
            iOffset = -1;
            jOffset = 1;
            break;
        case 3:
            jOffset = -1;
            break;
        case 4:
            jOffset = 1;
            break;
        case 5:
            iOffset = 1;
            jOffset = -1;
            break;
        case 6:
            iOffset = 1;
            break;
        case 7:
            iOffset = 1;
            jOffset = 1;
            break;
    }

    nextI = i + iOffset;
    nextJ = j + jOffset;
    
    if (nextI < 0 || nextI >= this->size || nextJ < 0 || nextJ >= this->size)
    return false;
    else if (this->grid[nextI][nextJ] == (!p) && this->grid[i][j] == p)
        return canFlip(p, nextI, nextJ, d);
    else if (this->grid[nextI][nextJ] == (!p) && this->grid[i][j] == OPEN) {
        bool nextCanFlip = canFlip(p, nextI, nextJ, d);
        if (nextCanFlip)
            this->grid[i][j] = p;
        return nextCanFlip;
    }
    else if (this->grid[nextI][nextJ] == (!p) && this->grid[i][j] == (!p)) {
        bool nextCanFlip = canFlip(p, nextI, nextJ, d);
        if (nextCanFlip)
            flip(i, j);
        return nextCanFlip;
    }
    else if (this->grid[nextI][nextJ] == p && this->grid[i][j] == (!p)) {
        flip(i, j);
        return true;
    }
    return false;
}

bool Othello::checkCanFlip(int p, int i, int j, int d) {
    int iOffset = 0, jOffset = 0, nextI, nextJ;
    switch (d) {
        case 0:
            iOffset = -1;
            jOffset = -1;
            break;
        case 1:
            iOffset = -1;
            break;
        case 2:
            iOffset = -1;
            jOffset = 1;
            break;
        case 3:
            jOffset = -1;
            break;
        case 4:
            jOffset = 1;
            break;
        case 5:
            iOffset = 1;
            jOffset = -1;
            break;
        case 6:
            iOffset = 1;
            break;
        case 7:
            iOffset = 1;
            jOffset = 1;
            break;
    }

    nextI = i + iOffset;
    nextJ = j + jOffset;

    if (nextI < 0 || nextI >= this->size || nextJ < 0 || nextJ >= this->size) {
        return false;
    }
    
    if (this->grid[nextI][nextJ] == OPEN)
        return false;
    else if (this->grid[nextI][nextJ] == p && this->grid[i][j] == OPEN) {
        return false;
    }
    else if (this->grid[nextI][nextJ] == p && this->grid[i][j] == (!p)) {
        return true;
    }
    return checkCanFlip(p, nextI, nextJ, d);
}

bool Othello::checkTurn(int p, int i, int j) {
    bool valid = false;
    if (this->grid[i][j] != OPEN)
        return false;
    for (int k = 0; k < 8; k++)
        if (canFlip(p, i, j, k))
            valid = true;
    return valid;
}

int Othello::makeTurn(int i, int j) {
    if (checkWin() == -1) {
        if (!checkTurn(current, i, j)) {
            return -3;
        }
        if (current == WHITE) {
            current = BLACK;
        } else {
            current = WHITE;
        }
        if (!hasTurn(current)) {
            if (current == WHITE) {
                current = BLACK;
            } else {
                current = WHITE;
            }
        }
    }
    return checkWin();
}

void Othello::reset() {
    for (int i = 0; i < size; i++) {
        this->grid[i] = new int[size];
        for (int j = 0; j < size; j++)
            this->grid[i][j] = OPEN;
    }
    int startIndex = (this->size - 1) / 2;
    this->grid[startIndex][startIndex + 1] = BLACK;
    this->grid[startIndex][startIndex] = WHITE;
    this->grid[startIndex + 1][startIndex] = BLACK;
    this->grid[startIndex + 1][startIndex + 1] = WHITE;
    this->numWhite = 2;
    this->numBlack = 2;
    this->current = BLACK;
}
