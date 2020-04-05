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
static char startStr[10];
static int level;
static char scoreStr[10];
static int score;
static int linesCleared;
int speed;
int levelScore = 100;
int pieceFlag = 0;
Tetromino storedPiece;
int storeIsEmpty = 1;


// reset state to beginning
void reset(void) {

    // initialize game vars
    state = START;
    level = 1;
    score = 0;
    linesCleared = 0;
    speed = 37;
    int src = 0;
	DMA[3].src = &src;
	DMA[3].dst = &grid[0];
	DMA[3].cnt = 16 | DMA_ON | DMA_SOURCE_FIXED;

    // draw start image and text
	drawFullScreenImageDMA(startImage);
    sprintf(startStr, "Press ENTER to Start");
    drawString(120, 60, startStr, WHITE);
}

// method to enter gameplay
void start(void) {

    // enter game state and draw background and grid
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
    
    // create current piece
	nextPiece = generatePiece();
	drawNext();

    // draw upcoming piece in top left
	currPiece = generatePiece();
	drawPiece();

    // draw storage box
    drawStore();
}

// end of game
void end(void) {

    // player wins if score is 1000 or higher
    if (score >= 1000) {
        state = WIN;
        drawFullScreenImageDMA(winner);
    } else {
        state = LOSE;
        drawFullScreenImageDMA(loser);
    }
}

// make instance of piece struct and initialize parameters
Tetromino generatePiece(void) {
    Tetromino piece;

    // set coordinates and paramters for piece
    setPiece(&piece, rand() % 7, 0);
    piece.x = 4;
	piece.y = 0;
	return piece;
}

// draw the current piece at the top of the grid
void drawPiece(void) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {

            // if the space in 4x4 piece area is shaded for this rotation, then shade in that block
            if (currPiece.space[i][j] == 1) {
                drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 60 + BLOCKLENGTH * (currPiece.x + j), BLOCKLENGTH, BLOCKLENGTH, currPiece.color);
            }
        }
    }
}


// rotate current piece
void rotatePiece(int direction) {

    // set rotated piece's coordinates to that of current piece
    Tetromino rotatedPiece;
    rotatedPiece.x = currPiece.x;
    rotatedPiece.y = currPiece.y;

    // if direction is 1, rotate right
    // if direction is 0, rotate left
    if (direction == 1){
        setPiece(&rotatedPiece, currPiece.type, (currPiece.rotation + 1) % 4);
    } else {
        if (currPiece.rotation == 0) {
            setPiece(&rotatedPiece, currPiece.type, 3);
        } else {
            setPiece(&rotatedPiece, currPiece.type, currPiece.rotation - 1);
        }
    }

    // as long as this rotation doesn't cause the block to collide with anything, change rotation
    if (collision(rotatedPiece) == 0) {
        erasePiece();
        currPiece = rotatedPiece;
        drawPiece();
    }
}

// draw the upcoming piece in the top left corner
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

// move horizontally and vertically by certain amount
void movePiece(int dx, int dy) {
    Tetromino movedPiece = currPiece;
    movedPiece.x += dx;
    movedPiece.y += dy;

    // if the moved piece doesnt collide with anything, then change current piece to moved piece
    if (collision(movedPiece) == 0) {
        erasePiece();
        currPiece = movedPiece;
        drawPiece();

    // if there is a collision caused by the block moving down
    } else if (collision(movedPiece) == 1 && dy == 1) {
        // update piece flag
        pieceFlag = 0;

        // update grid
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currPiece.space[i][j] == 1) grid[currPiece.y + i][currPiece.x + j] = currPiece.color;
            }
        }
        int init = currPiece.y;
        int exit = init + 4;
        currPiece = nextPiece;

        // check for full rows
        if (collision(currPiece) == 0) {
            drawPiece();
            nextPiece = generatePiece();
            drawNext();
            int fullRows = 0;
            int fullFlag = 1;
            for (int i = init; i < exit; i++) {
                fullFlag = 1;

                // if any of the blocks aren't filled in, then the row is not full
                for (int j = 0; j < 10; j++) {
                    if (grid[i][j] == 0) {
                        fullFlag = 0;
                        break;
                    }
                }

                // if row is full, then clear lines
                if (fullFlag == 1) {
                    clearLines(i);
                    fullRows++;
                }
            }

            // update score if lines are cleared
            if (fullRows > 0) updateLevel(fullRows);

        // if the piece has collision despite having reached a halt, then end the game
        } else {
            end();
        }
    }
}

// clear line of blocks and move everything above it down
void clearLines(int i) {
    u16 moveDown;
    for (int k = 0; k < 10; k++) {
        for (int a = i; a >= 0; a--) {
            moveDown = grid[a][k];

            // if grid is not at top, then set it to whatever is above it
            if (a == 0) {
                grid[a][k] = 0;
            } else {
                grid[a][k] = grid[a - 1][k];
            }

            // checks if the rest of the board is clear to stop updating for efficiency
            if (moveDown != grid[a][k]) {

                // updates pieces on screen
                if (grid[a][k] != 0) {
                    drawRectDMA(BLOCKLENGTH * a, 60 + BLOCKLENGTH * k, BLOCKLENGTH, BLOCKLENGTH, grid[a][k]);
                } else {
                    drawRectDMA(BLOCKLENGTH * a, 60 + BLOCKLENGTH * k, BLOCKLENGTH, BLOCKLENGTH, GRAY);
                }
            }
        }
    }
}

// updates level based on lines cleared
void updateLevel(int fullRows) {
    drawRectDMA(100, 150, 140, BLOCKLENGTH, BACKGROUND);
    score += (fullRows + 1) * (38 - speed) * 10;

    // every time score increases by 100, level increases and game speeds up
    if (score >= levelScore + 100) {
        level++;
        if (speed > 1) speed -= 3;
        levelScore += 100;
    }

    // show level and score on screen
    sprintf(scoreStr, "Score: %d", score);
    drawString(100, 150, scoreStr, WHITE);
    drawRectDMA(120, 150, 140, BLOCKLENGTH, BACKGROUND);
    sprintf(levelStr, "Level: %d", level);
    drawString(120, 150, levelStr, WHITE);
}

// set piece to a certain type and rotation from the pieces array
void setPiece(Tetromino *piece, int type, int rotation) {

    // copy piece data into spaces array for that instance of piece struct
    DMA[3].src = 0;
	DMA[3].dst = &piece->space[0][0];
	DMA[3].cnt = 16 | DMA_ON | DMA_SOURCE_FIXED | DMA_32;
	piece->rotation = rotation;

    // get piece data
	for (int i = 0; i < 4; i++) {
        int x = PIECES[type][rotation][i][0];
        int y = PIECES[type][rotation][i][1];
		piece->space[x][y] = 1;
	}

    // set piece color based on type
    if (type == 0) piece->color = YELLOW;
    else if (type == 1) piece->color = CYAN;
    else if (type == 2) piece->color = MAGENTA;
    else if (type == 3) piece->color = BLUE;
    else if (type == 4) piece->color = ORANGE;
    else if (type == 5) piece->color = GREEN;
    else piece->color = RED;
	piece->type = type;
}


// check for collision with left boundary, right boundary, bottom boundary of grid and also with other pieces
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

// wipe current piece off of the grid, used to change moved/rotated piece
void erasePiece(void) {
    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (currPiece.space[i][j] != 0) {
				drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 60 + BLOCKLENGTH * (j + currPiece.x), BLOCKLENGTH, BLOCKLENGTH, GRAY);
			}
		}
	}
}

// hard drop tetris piece
void hardDrop(void) {
    int nextFlag = 0;

    // checks if space below is open and then drops
    while (nextFlag == 0) {
        movePiece(0, 1);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currPiece.space[i][j] != 0) {
                    if ((grid[currPiece.y + i + 1][currPiece.x + j] != 0) || (currPiece.y + i + 1 >= 20)) {
                        nextFlag = 1;
                    }
                }
            }
        }
    }
}

// store piece for later use
void storePiece(int storeFlag) {
    if (storeFlag == 0) {
        Tetromino tempStore = storedPiece;
        storedPiece = currPiece;

        // stores current piece in storage
        u16 color;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currPiece.space[i][j] == 1) {
                    color = currPiece.color;
                } else {
                    color = GRAY;
                }
                drawRectDMA(BLOCKLENGTH * i, 120 + BLOCKLENGTH * (4 + j), BLOCKLENGTH, BLOCKLENGTH, color);
            }
        }
        erasePiece();
        if (storeIsEmpty == 1) currPiece = nextPiece;
        else currPiece = tempStore;
        drawPiece();
        nextPiece = generatePiece();
        drawNext();
        pieceFlag = 1;
        storeIsEmpty = 0;
    }
}

// draws storage box
void drawStore(void) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            drawRectDMA(BLOCKLENGTH * (currPiece.y + i), 120 + BLOCKLENGTH * (currPiece.x + j), BLOCKLENGTH, BLOCKLENGTH, GRAY);
        }
    }
}