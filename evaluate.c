#include <stdio.h>
#include "defs.h"

// basic evaluation of different squares for a pawn
const int PawnTable[64] = {
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	, // row 1
    10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
    5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
    0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
    5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
    10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
    20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	 // row 8
};

// basic evaluation of different squares for a knight
const int KnightTable[64] = {
    0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
    0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
    0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
    0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
    5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
    5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

// basic evaluation of different squares for a bishop
const int BishopTable[64] = {
    0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
    0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
    0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
    0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

// basic evaluation of different squares for a rook
const int RookTable[64] = {
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
    25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
    0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

// basic evaluation of different squares for a king end game
const int KingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};

// basic evaluation of different squares for a king oppening
const int KingO[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70		
};

// array to mirrow the inputed sq to black's pov
const int Mirror64[64] = {
    56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
	48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
	40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
	32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
	24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
	16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
	8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
	0	,	1	,	2	,	3	,	4	,	5	,	6	,	7		
};

#define MIRROR64(sq) (Mirror64[(sq)]) // macro to mirror board sq

// return the evaluation of the position
int EvalPosition(const S_BOARD *pos) {
    int piece;
    int piece_num;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    piece = wP;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score += PawnTable[SQ64(sq)];
    }

    piece = bP;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score -= PawnTable[MIRROR64(SQ64(sq))];
    }

    piece = wH;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score += KnightTable[SQ64(sq)];
    }

    piece = bH;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score -= KnightTable[MIRROR64(SQ64(sq))];
    }

    piece = wB;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score += BishopTable[SQ64(sq)];
    }

    piece = bB;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score -= BishopTable[MIRROR64(SQ64(sq))];
    }

    piece = wR;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score += RookTable[SQ64(sq)];
    }

    piece = bR;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score -= RookTable[MIRROR64(SQ64(sq))];
    }

    if(pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}