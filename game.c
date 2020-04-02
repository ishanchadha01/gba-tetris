#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "images/loser.h"
#include "images/winner.h"
#include "images/startImage.h"

extern const int PIECES[7][4][4][2];
GBAState state;
u16 grid[20][10];
Tetromino currPiece;
Tetromino nextPiece;
static char levelStr[10];
static int level;
static char scoreStr[10];
static int score;
static int linesCleared;
int speed;
int levelScore = 100;

void reset(void) {
    state = START;
    level = 1;
    score = 0;
    linesCleared = 0;
    speed = 37;
    int src = 0;
	DMA[3].src = &src;
	DMA[3].dst = &grid[0];
	DMA[3].cnt = 16 | DMA_ON | DMA_SOURCE_FIXED;
	drawFullScreenImageDMA(startImage);
}

void start(void) {
    state = PLAY;
    srand(0);
    fillScreenDMA(BACKGROUND);
    drawRectDMA(0, 60, BLOCKLENGTH * 10, BLOCKLENGTH * 20, GRAY);

	// initial score text
	sprintf(scoreStr, "Score: %d", 0);
	drawString(100, 150, scoreStr, WHITE);

	// initial level (calculated from the speed) text
	sprintf(levelStr, "Level: %d", 1);
	drawString(120, 150, levelStr, WHITE);
    
	nextPiece = generatePiece();
	drawNext();
	currPiece = generatePiece();
	drawPiece();
}

void end(void) {
    if (score >= 1000) {
        state = WIN;
        drawFullScreenImageDMA(winner);
    } else {
        state = LOSE;
        drawFullScreenImageDMA(loser);
    }
}
    
Tetromino generatePiece(void) {
    Tetromino piece;
    setPiece(&piece, rand() % 7, 0);
    piece.x = 4;
	piece.y = 0;
	return piece;
}

void drawPiece(void) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currPiece.space[i][j] == 1) {
                drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 60 + BLOCKLENGTH * (currPiece.x + j), BLOCKLENGTH, BLOCKLENGTH, currPiece.color);
            }
        }
    }
}

void rotatePiece(int direction) {
    Tetromino rotatedPiece;
    rotatedPiece.x = currPiece.x;
    rotatedPiece.y = currPiece.y;
    if (direction == 1){
        setPiece(&rotatedPiece, currPiece.type, (currPiece.rotation + 1) % 4);
    } else {
        if (currPiece.rotation == 0) {
            setPiece(&rotatedPiece, currPiece.type, 3);
        } else {
            setPiece(&rotatedPiece, currPiece.type, currPiece.rotation - 1);
        }
    }
    if (collision(rotatedPiece) == 0) {
        erasePiece();
        currPiece = rotatedPiece;
        drawPiece();
    }
}

void drawNext(void) {
    u16 color;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (nextPiece.space[i][j] == 1) {
                color = nextPiece.color;
            } else {
                color = GRAY;
            }
            drawRectDMA(BLOCKLENGTH * (nextPiece.y + i), BLOCKLENGTH * (nextPiece.x + j), BLOCKLENGTH, BLOCKLENGTH, color);
        }
    }
}

void movePiece(int dx, int dy) {
    Tetromino movedPiece = currPiece;
    movedPiece.x += dx;
    movedPiece.y += dy;
    if (collision(movedPiece) == 0) {
        erasePiece();
        currPiece = movedPiece;
        drawPiece();
    } else if (collision(movedPiece) == 1 && dy == 1) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currPiece.space[i][j] == 1) grid[currPiece.y + i][currPiece.x + j] = currPiece.color;
            }
        }
        int init = currPiece.y;
        int exit = init + 4;
        currPiece = nextPiece;
        u16 moveDown;
        if (collision(currPiece) == 0) {
            drawPiece();
            nextPiece = generatePiece();
            drawNext();
            int fullRows = 0;
            int fullFlag = 1;
            for (int i = init; i < exit; i++) {
                fullFlag = 1;
                for (int j = 0; j < 10; j++) {
                    if (grid[i][j] == 0) {
                        fullFlag = 0;
                        break;
                    }
                }
                if (fullFlag == 1) {
                    for (int k = 0; k < 10; k++) {
                        for (int a = i; a >= 0; a--) {
                            moveDown = grid[a][k];
                            if (a == 0) {
                                grid[a][k] = 0;
                            } else {
                                grid[a][k] = grid[a - 1][k];
                            }
                            if (moveDown != grid[a][k]) {
                                if (grid[a][k] != 0) {
                                    drawRectDMA(BLOCKLENGTH * a, 60 + BLOCKLENGTH * k, BLOCKLENGTH, BLOCKLENGTH, grid[a][k]);
                                } else {
                                    drawRectDMA(BLOCKLENGTH * a, 60 + BLOCKLENGTH * k, BLOCKLENGTH, BLOCKLENGTH, GRAY);
                                }
                            }
                        }
                    }
                    fullRows++;
                }
            }
            if (fullRows > 0) {
                drawRectDMA(100, 150, 140, BLOCKLENGTH, BACKGROUND);
                score += (fullRows + 1) * (38 - speed) * 10;
                if (score >= levelScore + 100) {
                    level++;
                    if (speed > 1) speed--;
                    levelScore += 100;
                }
			    sprintf(scoreStr, "Score: %d", score);
			    drawString(100, 150, scoreStr, WHITE);
				drawRectDMA(120, 150, 140, BLOCKLENGTH, BACKGROUND);
				sprintf(levelStr, "Level: %d", level);
				drawString(120, 150, levelStr, WHITE);
			}
        } else {
            end();
        }
    }
}

void setPiece(Tetromino *piece, int type, int rotation) {
    DMA[3].src = 0;
	DMA[3].dst = &piece->space[0][0];
	DMA[3].cnt = 16 | DMA_ON | DMA_SOURCE_FIXED | DMA_32;
	piece->rotation = rotation;
	for (int i = 0; i < 4; i++) {
        int x = PIECES[type][rotation][i][0];
        int y = PIECES[type][rotation][i][1];
		piece->space[x][y] = 1;
	}
	switch (type) {
        case 0:
            piece->color = YELLOW;
            break;
        case 1:
            piece->color = CYAN;
            break;
        case 2:
            piece->color = MAGENTA;
            break;
        case 3:
            piece->color = BLUE;
            break;
        case 4:
            piece->color = ORANGE;
            break;
        case 5:
            piece->color = GREEN;
            break;
        case 6:
            piece->color = RED;
            break;
	}
	piece->type = type;
}

int collision(Tetromino aPiece) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (aPiece.space[i][j] != 0) {
                if ((grid[aPiece.y + i][aPiece.x + j] != 0) || (aPiece.x + j < 0) || (aPiece.x + j >= 10) || (aPiece.y + i >= 20)) {
					return 1;
                }
            }
        }
    }
    return 0;
}

void erasePiece(void) {
    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (currPiece.space[i][j] != 0) {
				drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 60 + BLOCKLENGTH * (j + currPiece.x), BLOCKLENGTH, BLOCKLENGTH, GRAY);
			}
		}
	}
}