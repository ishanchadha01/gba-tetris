#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "gba.h"

const int PIECES[7][4][4][2] = {
    {
		// each row is a different rotation of the O tetromino in a 4x4 block space
		{{0, 1}, {0, 2}, {1, 2}, {1, 1}},
		{{0, 1}, {0, 2}, {1, 2}, {1, 1}},
		{{0, 1}, {0, 2}, {1, 2}, {1, 1}},
		{{0, 1}, {0, 2}, {1, 2}, {1, 1}}
	},
	{
		// each row is a different rotation of the I tetromino in a 4x4 block space
		{{0, 2}, {1, 2}, {2, 2}, {3, 2}},
		{{2, 0}, {2, 1}, {2, 2}, {2, 3}},
		{{0, 1}, {1, 1}, {2, 1}, {3, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {1, 3}}
	},
    {
		// each row is a different rotation of the T tetromino in a 4x4 block space
        {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
		{{0, 1}, {1, 0}, {1, 1}, {2, 1}},
		{{0, 1}, {1, 0}, {1, 2}, {1, 1}},
		{{0, 1}, {1, 1}, {1, 2}, {2, 1}}
	},

    //flipped tetrominoes J/L and S/Z
    {
		// each row is a different rotation of the J tetromino in a 4x4 block space
        {{0, 1}, {1, 1}, {2, 1}, {2, 0}},
		{{0, 0}, {1, 0}, {1, 2}, {1, 1}},
		{{0, 1}, {1, 1}, {0, 2}, {2, 1}},
		{{1, 0}, {1, 1}, {1, 2}, {2, 2}}
	},
	{
		// each row is a different rotation of the L tetromino in a 4x4 block space
		{{0, 1}, {1, 1}, {2, 1}, {2, 2}},
		{{1, 0}, {1, 1}, {1, 2}, {2, 0}},
		{{0, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{0, 2}, {1, 1}, {1, 2}, {1, 0}}
	},
	{
		// each row is a different rotation of the S tetromino in a 4x4 block space
		{{1, 1}, {0, 2}, {1, 0}, {0, 1}},
		{{0, 1}, {1, 1}, {1, 2}, {2, 2}},
		{{1, 1}, {1, 2}, {2, 0}, {2, 1}},
		{{1, 0}, {0, 0}, {1, 1}, {2, 1}},
	},
	{
		// each row is a different rotation of the Z tetromino in a 4x4 block space
		{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
		{{0, 2}, {1, 1}, {2, 1}, {1, 2}},
		{{1, 0}, {1, 1}, {2, 1}, {2, 2}},
		{{1, 0}, {0, 1}, {1, 1}, {2, 0}}
	}
};