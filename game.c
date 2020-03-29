#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

extern const int PIECES[7][4][4][2];
extern GBAState state;
u16 grid[20][10];
Tetromino currPiece;
Tetromino nextPiece;
static int level;
static int score;
static int frame;
static int linesCleared;
static int frameSpeed;

void reload() {
    switch(state) {
        case START:
            frame++;
            break;
        case PLAY:
            if (frame != frameSpeed) {
                frame++;
            } else {
                movePiece(0, 1);
            }
            break;
        case WIN:
            frame++;
            break;
        case LOSE:
            frame++;
            break;
    }
}

void reset() {
    state = START;
    level = 1;
    score = 0;
    frame = 0;
    linesCleared = 0;
    frameSpeed = 40;
    int src = 0;
	DMA[3].src = &src;
	DMA[3].dst = &grid[0];
	DMA[3].cnt = 16 | DMA_SOURCE_FIXED | DMA_16 | DMA_ON;
	// drawImage3(0, 0, START_IMAGE_WIDTH, START_IMAGE_HEIGHT, start_image);
}

void start() {
    state = PLAY;
    srand(frame);
    drawRectDMA(0, 0, 240, 160, BACKGROUND);
    drawRectDMA(0, 40, BLOCKLENGTH * 10, BLOCKLENGTH * 20, GRAY);
	// drawImageDMA(30, 150, CORGI_IMAGE_WIDTH, CORGI_IMAGE_HEIGHT, parth_image);

	// initial score text
	sprintf(scoreString, "Score: %d", 0);
	drawString(100, 150, scoreString, WHITE);

	// initial level (calculated from the speed) text
	sprintf(levelString, "Level: %d", 1);
	drawString(120, 150, levelString, WHITE);
	currPiece = generatePiece();
	drawPiece();
	nextPiece = generatePiece();
	drawNext();
    frame = 0;
}

void end() {
    if (score >= 1000) {
        state = WIN;
    } else {
        state = LOSE;
    }
    //drawImageDMA(0, 0, GAMEOVER_IMAGE_WIDTH, GAMEOVER_IMAGE_HEIGHT, gameover_image);
}
    
Tetromino generatePiece() {
    Tetromino piece;
    setPiece(&piece, rand() % 7, 0);
    piece.x = 8;
	piece.y = 0;
	return piece;
}

void drawPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currPiece.space[i][j] == 1) {
                drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 40 + BLOCKLENGTH * (currPiece.x + j), BLOCKLENGTH, BLOCKLENGTH, currPiece.color);
            }
        }
    }
}

void rotatePiece(int direction) {
    Tetromino rotatedPiece;
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

void drawNext() {
    u16 color;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (nextPiece.space[i][j] == 1) {
                color = nextPiece.color;
            } else {
                color = BACKGROUND;
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
    } else if (collision(movedPiece) && dy == 1) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currPiece.space[i][j] == 1) grid[currPiece.y + i][currPiece.x + j] = currPiece.color;
            }
        }
        currPiece = nextPiece;
        u16 moveDown;
        if (collision(currPiece) == 0) {
            drawPiece();
            nextPiece = generatePiece();
            drawNext();
            frame = 0;
            int fullRows = 0;
            int fullFlag = 1;
            for (int i = 0; i < 4; i++) {
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
                                grid [a][k] = grid[a - 1][k];
                            }
                            if (moveDown != grid[a][k]) {
                                if (grid[a][k] == 0) {
                                    drawRectDMA(BLOCKLENGTH * a, 40 + BLOCKLENGTH * k, BLOCKLENGTH, BLOCKLENGTH, BACKGROUND);
                                } else {
                                    drawRectDMA(BLOCKLENGTH * a, 40 + BLOCKLENGTH * k, BLOCKLENGTH, BLOCKLENGTH, GRAY);
                                }
                            }
                        }
                    }
                    fullRows++;
                }
            }
            if (fullRows > 0) {
                if (frameSpeed > 1) frameSpeed--;
				drawRectDMA(120, 150, 140, 8, GRAY);
				// print levelString "Level: %d", INIT_SPEED - speed + 1
				// drawString(120, 150, levelString, WHITE)
			}

			// add to the score and redraw it
			score += (fullRows + 1) * (40 - frameSpeed + 1);
			drawRect3(100, 150, 140, 8, GRAY);
			// print scoreString "Score: %d", score
			// drawString(100, 150, scoreString, WHITE)
        } else {
            end();
        }
    }
}

void setPiece(Tetromino *piece, int type, int rotation) {
    DMA[3].src = 0;
	DMA[3].dst = &piece->space[0][0];
	DMA[3].cnt = 16 | DMA_SOURCE_FIXED | DMA_32 | DMA_ON;
	piece->rotation = rotation;
	for (int i = 0; i < 4; i++) {
		piece->space[PIECES[type][rotation][i][0]]
            [PIECES[type][rotation][i][1]] = 1;
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
                if ((grid[aPiece.y + i][aPiece.x + j] != 0)
                    || (aPiece.x < 0) || (aPiece.x + j >= 10)
                    || (aPiece.y + i >= 20)) return 1;
            }
        }
    }
}

void erasePiece() {
    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (currPiece.space[i][j] != 0) {
				drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 40 + BLOCKLENGTH * (j + currentBlock.x), BLOCKLENGTH, BLOCKLENGTH, GRAY);
			}
		}
	}
}