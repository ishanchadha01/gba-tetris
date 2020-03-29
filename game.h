#ifndef GAME_H
#define GAME_H
#define BLOCKLENGTH 8
#define BACKGROUND GRAY

#include "gba.h"

        typedef enum {
                START,
                PLAY,
                WIN,
                LOSE,
        } GBAState;

        typedef struct state {
                int currentState;
                int nextState;
        } State;

        typedef struct tetromino {
                u16 color;
                int type;
                int x;
                int y;
                int rotation;
                int space[4][4];
        } Tetromino;

        // Game actions
        void reload();
        void reset();
        void start();
        void end();

        // Piece actions
        Tetromino generatePiece();
        void drawPiece();
        void rotatePiece(int direction);
        void drawNext();
        void movePiece(int x, int y);
        void setPiece(Tetromino *piece, int type, int rotation);
        int collision(Tetromino collided);
        void erasePiece();

        // External vars
        extern int state;
        extern Tetromino currPiece;
        extern Tetromino nextPiece;
        extern const int PIECES[7][4][4][2];
        extern u16 grid[20][10];

#endif
