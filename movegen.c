#include <stdio.h>
#include "defs.h"

#define MOVE(from,to,capture,promotion,flag) ( (from) | ((to) << 7) | ((capture) << 14) | ((promotion) << 20) | (flag))
#define SQOFFBOARD(sq) (FilesBrd[(sq)]==OFFBOARD)

const int LoopSlidePiece[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};
const int LoopSlideIndex[2] = {0, 4}; // index for LoopSlidePiece array depending on side
const int LoopNonSlidePiece[6] = {wH, wK, 0, bH, bK, 0};
const int LoopNonSlideIndex[2] = {0, 3};

// direction by piece type
const int PieceDir[13][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12},
    {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},
    {-1, -10, 1, 10, -9, -11, 11, 9},
    {-1, -10, 1, 10, -9, -11, 11, 9},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12},
    {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},
    {-1, -10, 1, 10, -9, -11, 11, 9},
    {-1, -10, 1, 10, -9, -11, 11, 9}
};

const int NumDir[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8}; // number of possible directions by piece type

static void AddQuietMove (const S_BOARD *pos, int move, S_MOVELIST *list ){
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

static void AddCaptureMove (const S_BOARD *pos, int move, S_MOVELIST *list ){
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

static void AddEnPassantMove (const S_BOARD *pos, int move, S_MOVELIST *list ){
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

// handels promotions for white pawn capture moves
static void AddWhitePawnCapMove (const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    assert(PieceValidEmpty(cap));
    assert(SqOnBoard(from));
    assert(SqOnBoard(to));
    
    if(RanksBrd[from] == RANK_7) {
        AddCaptureMove(pos, MOVE(from,to,cap,wQ,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,wR,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,wB,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,wH,0), list);
    } else {
        AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}

// handles promotions for white pawns
static void AddWhitePawnMove (const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    if(RanksBrd[from] == RANK_7) {
        AddQuietMove(pos, MOVE(from,to,EMPTY,wQ,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,wR,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,wB,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,wH,0), list);
    } else {
        AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}

// handles promotions for black pawn capture moves
static void AddBlackPawnCapMove (const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    assert(PieceValidEmpty(cap));
    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    if(RanksBrd[from] == RANK_2) {
        AddCaptureMove(pos, MOVE(from,to,cap,bQ,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,bR,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,bB,0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,bH,0), list);
    } else {
        AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}

// handles promotions for white pawns
static void AddBlackPawnMove (const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    if(RanksBrd[from] == RANK_2) {
        AddQuietMove(pos, MOVE(from,to,EMPTY,bQ,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,bR,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,bB,0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,bH,0), list);
    } else {
        AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}

void GenerateAllMoves (const S_BOARD *pos,  S_MOVELIST *list ) {
    assert(CheckBoard(pos));

    list->count = 0;
    int piece = EMPTY;
    int side = pos->side;
    int sq = 0;
    int temp_sq= 0;
    int piece_num = 0;
    int dir = 0;
    int index = 0;
    int piece_index = 0;

    // Pawn and castling generation
    if(side == WHITE) {
        for(piece_num = 0; piece_num < pos->piece_num[wP]; ++piece_num) {
            sq = pos->piece_list[wP][piece_num];
            assert(SqOnBoard(sq));
            if(pos->pieces[sq + 10] == EMPTY) {
                AddWhitePawnMove(pos, sq, sq + 10, list);
                if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
                    AddQuietMove(pos,MOVE(sq, sq + 20, EMPTY, EMPTY, MFLAGPS), list);
                }
            }

            if(!SQOFFBOARD(sq + 9) && PieceColour[pos->pieces[sq + 9]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceColour[pos->pieces[sq + 11]] == BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
            }

            if(sq + 9 == pos->enPas) {
                AddCaptureMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
            }
            if(sq + 11 == pos->enPas) {
                AddCaptureMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
            }
        }

        if(pos->castle_perm & WKCA) {
            if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
                if(!SqAttacked(E1, BLACK, pos) && !SqAttacked(F1, BLACK, pos)) {
                    AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCASTLE), list);
                }
            }
        }

        if(pos->castle_perm & WQCA) {
            if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
                if(!SqAttacked(E1, BLACK, pos) && !SqAttacked(D1, BLACK, pos)) {
                    AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCASTLE), list);
                }
            }
        }

    } else {
        for(piece_num = 0; piece_num < pos->piece_num[bP]; ++piece_num) {
            sq = pos->piece_list[bP][piece_num];
            assert(SqOnBoard(sq));
            if(pos->pieces[sq - 10] == EMPTY) {
                AddBlackPawnMove(pos, sq, sq - 10, list);
                if(RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
                    AddQuietMove(pos,MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS), list);
                }
            }

            if(!SQOFFBOARD(sq - 9) && PieceColour[pos->pieces[sq - 9]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if(!SQOFFBOARD(sq - 11) && PieceColour[pos->pieces[sq - 11]] == WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
            }

            if(sq - 9 == pos->enPas) {
                AddCaptureMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
            }
            if(sq - 11 == pos->enPas) {
                AddCaptureMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
            }
        }

        if(pos->castle_perm & BKCA) {
            if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
                if(!SqAttacked(E8, WHITE, pos) && !SqAttacked(F8, WHITE, pos)) {
                    AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCASTLE), list);
                }
            }
        }

        if(pos->castle_perm & BQCA) {
            if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
                if(!SqAttacked(E8, WHITE, pos) && !SqAttacked(D8, WHITE, pos)) {
                    AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCASTLE), list);
                }
            }
        }
    }

    // Generation of sliding pieces
    piece_index = LoopSlideIndex[side];
    piece = LoopSlidePiece[piece_index++];

    while(piece != 0) {
        assert(PieceValid(piece));
       
        for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
            sq = pos->piece_list[piece][piece_num];
            assert(SqOnBoard(sq));

            for(index = 0; index < NumDir[piece]; ++index) {
                dir = PieceDir[piece][index];
                temp_sq = sq + dir;

                while(!SQOFFBOARD(temp_sq)) {
                    if(pos->pieces[temp_sq] != EMPTY) {
                        if(PieceColour[pos->pieces[temp_sq]] == side ^ 1) { // BLACK ^ 1 == WHITE and vice versa
                            AddCaptureMove(pos, MOVE(sq, temp_sq, pos->pieces[temp_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    AddQuietMove(pos, MOVE(sq, temp_sq, EMPTY, EMPTY, 0), list);
                    temp_sq += dir;
                }
            }
        }
        piece = LoopSlidePiece[piece_index++];
    }

    // Generation of non sliding pieces
    piece_index = LoopNonSlideIndex[side];
    piece = LoopNonSlidePiece[piece_index++];

    while(piece != 0) {
        assert(PieceValid(piece));

        for(piece_num = 0; piece_num < pos->piece_num[piece]; ++piece_num) {
            sq = pos->piece_list[piece][piece_num];
            assert(SqOnBoard(sq));

            for(index = 0; index < NumDir[piece]; ++index) {
                dir = PieceDir[piece][index];
                temp_sq = sq + dir;

                if(SQOFFBOARD(temp_sq)) {
                    continue;
                }

                if(pos->pieces[temp_sq] != EMPTY) {
                    if(PieceColour[pos->pieces[temp_sq]] == side ^ 1) { // BLACK ^ 1 == WHITE and vice versa
                        AddCaptureMove(pos, MOVE(sq, temp_sq, pos->pieces[temp_sq], EMPTY, 0), list);
                    }
                    continue;
                }
                AddQuietMove(pos, MOVE(sq, temp_sq, EMPTY, EMPTY, 0), list);
            }
        }

        piece = LoopNonSlidePiece[piece_index++];
    }

}