#ifndef GAME_H
#define GAME_H
#define BLOCKLENGTH 8
#define BACKGROUND BLACK

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
        void delay(int sec);
        void reload(void);
        void reset(void);
        void start(void);
        void end(void);

        // Piece actions
        Tetromino generatePiece(void);
        void drawPiece(void);
        void rotatePiece(int direction);
        void drawNext(void);
        void movePiece(int x, int y);
        void clearLines(int i);
        void updateLevel(int fullRows);
        void setPiece(Tetromino *piece, int type, int rotation);
        int collision(Tetromino collided);
        void erasePiece(void);
        void hardDrop(void);
        void storePiece(int storeFlag);
        void drawStore(void);

        // External vars
        extern Tetromino currPiece;
        extern Tetromino nextPiece;
        extern const int PIECES[7][4][4][2];
        extern u16 grid[20][10];
        extern const unsigned short loser[38400];
        extern const unsigned short winner[38400];
        extern const unsigned short startImage[38400];
        extern int frameSpeed;

#endif
