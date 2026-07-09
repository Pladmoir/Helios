#include <stdio.h>
#include <stdbool.h>
#include "defs.h"

const int PawnIsolated = -10;
const int PawnPassed[8] = {0, 5, 10, 20, 35, 60, 100, 200}; //indexed by rank
const int RookOpenFile = 10;
const int RookSemiOpenFile = 5;
const int QueenOpenFile = 5;
const int QueenSemiOpenFile = 3;
const int BishopPair = 30;

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

int MaterialDraw(const S_BOARD *pos) {
    if (!pos->piece_num[wR] && !pos->piece_num[bR] && !pos->piece_num[wQ] && !pos->piece_num[bQ]) {
        if (!pos->piece_num[bB] && !pos->piece_num[wB]) {
            if (pos->piece_num[wH] < 3 && pos->piece_num[bH] < 3) {return true;}    
        } else if (!pos->piece_num[wH] && !pos->piece_num[bH]) {
            if (abs(pos->piece_num[wB] - pos->piece_num[bB]) < 2) {return true;}
        } else if ((pos->piece_num[wH] < 3 && !pos->piece_num[wB]) || (pos->piece_num[wB] == 1 && !pos->piece_num[wH])) {
            if ((pos->piece_num[bH] < 3 && !pos->piece_num[bB]) || (pos->piece_num[bB] == 1 && !pos->piece_num[bH])) { return true;}
        }
    } else if (!pos->piece_num[wQ] && !pos->piece_num[bQ]) {
        if (pos->piece_num[wR] == 1 && pos->piece_num[bR] == 1) {
            if (((pos->piece_num[wH] + pos->piece_num[wB]) < 2) && ((pos->piece_num[bH] + pos->piece_num[bB]) < 2)) { return true;}
        } else if (pos->piece_num[wR] == 1 && !pos->piece_num[bR]) {
            if ((pos->piece_num[wH] + pos->piece_num[wB] == 0) && (((pos->piece_num[bH] + pos->piece_num[bB]) ==  1) || ((pos->piece_num[bH] + pos->piece_num[bB]) ==  2))) { return true;}
        } else if (pos->piece_num[bR] == 1 && !pos->piece_num[wR]) {
             if ((pos->piece_num[bH] + pos->piece_num[bB] == 0) && (((pos->piece_num[wH] + pos->piece_num[wB]) ==  1) || ((pos->piece_num[wH] + pos->piece_num[wB]) ==  2))) { return true;}
        }
    } 
    return false;
}

#define ENDGAME_MAT (1 * PieceValue[wR] + 2 * PieceValue[wH] + 2 * PieceValue[wP])
// return the evaluation of the position
int EvalPosition(const S_BOARD *pos) {
    int piece;
    int piece_num;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];

    if (!pos->piece_num[wP] && !pos->piece_num[bP] && MaterialDraw(pos) == true) {
        return 0;
    }

    piece = wP;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score += PawnTable[SQ64(sq)];

        if((IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            //printf("wP Iso:%s\n",PrSq(sq));
            score += PawnIsolated;
        }

        if((WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            //printf("wP Passed:%s\n",PrSq(sq));
            score += PawnPassed[RanksBrd[sq]];
        }
    }

    piece = bP;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score -= PawnTable[MIRROR64(SQ64(sq))];
        if((IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
            //printf("bP Iso:%s\n",PrSq(sq));
            score -= PawnIsolated;
        }

        if((BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
            //printf("bP Passed:%s\n",PrSq(sq));
            score -= PawnPassed[7 - RanksBrd[sq]];
        }
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
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score += RookOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            score += RookSemiOpenFile;
        }
    }

    piece = bR;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        score -= RookTable[MIRROR64(SQ64(sq))];
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score -= RookOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            score -= RookSemiOpenFile;
        }
    }

    piece = wQ;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score += QueenOpenFile;
        } else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
            score += QueenSemiOpenFile;
        }
    }

    piece = bQ;
    for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
        sq = pos->piece_list[piece][piece_num];
        assert(SqOnBoard(sq));
        if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
            score -= QueenOpenFile;
        } else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
            score -= QueenSemiOpenFile;
        }
    }

    piece = wK;
    sq = pos->piece_list[piece][0];

    if ((pos->material[BLACK] <= ENDGAME_MAT)) {
        score += KingE[SQ64(sq)];
    } else {
        score += KingO[SQ64(sq)];
    }

    piece = bK;
    sq = pos->piece_list[piece][0];

    if ((pos->material[WHITE] <= ENDGAME_MAT)) {
        score -= KingE[MIRROR64(SQ64(sq))];
    } else {
        score -= KingO[MIRROR64(SQ64(sq))];
    }

    if (pos->piece_num[wB] >= 2) score += BishopPair;
    if (pos->piece_num[bB] >= 2) score -= BishopPair;

    if (pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }
}