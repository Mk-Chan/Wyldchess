#include "defs.h"
#include "eval.h"
#include "position.h"

int piece_val[7] = {
	0,
	0,
	200,
	400,
	410,
	1000,
	2100
};

int psq_val[8][64] = {
	{ 0 },
	{ 0 },
	{	 // Pawn
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	},
	{	 // Knight
		-50, -30, -10, -10, -10, -20, -30, -50,
		-30, -10,   0,  10,  10,   0, -10, -30,
		-10,  10,  20,  25,  25,  20,  10, -10,
		  0,  10,  30,  35,  35,  30,  10,   0,
		  0,  15,  30,  40,  40,  30,  15,   0,
		  0,  15,  40,  50,  50,  40,  15,   0,
		-10,  -5,   5,   5,   5,   5,  -5, -10,
		-50, -30, -10, -10, -10, -20, -30, -50
	},
	{	 // Bishop
		-50, -30, -10, -10, -10, -10, -30, -50,
		 10,  20,  30,  20,  20,  30,  20,  10,
		 10,  40,  30,  35,  35,  30,  40,  10,
		 25,  30,  40,  40,  40,  40,  30,  25,
		 20,  40,  30,  30,  30,  30,  40,  20,
		 10,  20,  25,  20,  20,  25,  20,  10,
		-10,  -5,   0,   0,   0,   0,  -5, -10,
		-50, -30, -10, -10, -10, -10, -30, -50
	},
	{	 // Rook
		-20, -20, -10,   0,   0, -20, -20, -20,
		-20, -20, -10,   5,   5, -20, -20, -20,
		-10, -10,  -5,   5,   5,  -5, -10, -10,
		  0,   0,   5,   5,   5,   5,   0,   0,
		  0,   0,   5,   5,   5,   5,   0,   0,
		  0,   0,   5,   5,   5,   5,   0,   0,
		 10,  10,  15,  15,  15,  15,  10,  10,
		-10, -10,  -5,   5,   5,  -5, -10, -10
	},
	{
		 // Queen
		-50, -30, -10, -10, -10, -20, -30, -50,
		-30, -10,   0,  20,  20,   0, -10, -30,
		-10,  10,  20,  25,  25,  20,  10, -10,
		  0,  10,  30,  35,  35,  30,  10,   0,
		  0,  15,  30,  40,  40,  30,  15,   0,
		  0,  15,  40,  50,  50,  40,  15,   0,
		-10,  -5,   5,   5,   5,   5,  -5, -10,
		-50, -30, -10, -10, -10, -20, -30, -50
	},
	{
		 // King
		 30,  35,  10, -10, -10,  10,  35,  30,
		 10,  20,   0, -15, -15,   0,  20,  10,
		-20, -25, -30, -35, -35, -30, -25, -20,
		-40, -50, -60, -70, -70, -60, -50, -40,
		-70, -80, -90, -90, -90, -90, -80, -70,
		-70, -80, -90, -90, -90, -90, -80, -70,
		-70, -80, -90, -90, -90, -90, -80, -70,
		-70, -80, -90, -90, -90, -90, -80, -70
	}
};

int evaluate(Position* const pos)
{
	int eval = pos->state->piece_psq_eval[WHITE] - pos->state->piece_psq_eval[BLACK];
	return pos->stm == WHITE ? eval : -eval;
}
