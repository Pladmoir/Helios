
#include "defs.h"
#include <stdbool.h>

#define HASH_PIECE(piece,sq) (pos->pos_key ^= (PieceKeys[(piece)][(sq)])) 
#define HASH_CA (pos->pos_key ^= (CastleKey[(pos->castle_perm)])) 
#define HASH_SIDE (pos->pos_key ^= (SideKey)) 
#define HASH_EP (pos->pos_key ^= (PieceKeys[EMPTY][(pos->enPas)])) 

void TakeMove(S_BOARD *pos);

const int CastlePerm[120] = //bits to & with our castle permission to update it
{
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 7, 15, 15, 15, 3, 15, 15, 11, 15, // 3  = 0011 when added removes all black side castling permission
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

static void ClearPiece(const int sq, S_BOARD *pos) {
    assert(SqOnBoard(sq));
    int piece = pos->pieces[sq];
    assert(PieceValid(piece));
    int col = PieceColour[piece];
    int index = 0;
    int temp_piece_num = -1;

    HASH_PIECE(piece,sq); // hash piece out

    pos->pieces[sq] = EMPTY;
    pos->material[col] -= PieceValue[piece];

    if (PieceBig[piece]) {
        pos->big_piece[col] --;
        if(PieceMajor[piece]) {
            pos->maj_piece[col]--;
        } else {
             pos->min_piece[col]--;
        }
    } else {
        CLRBIT(pos->pawns[col], SQ64(sq));
        CLRBIT(pos->pawns[BOTH], SQ64(sq));
    }

    for(index = 0; index < pos->piece_num[piece]; ++index) {
        if(pos->piece_list[piece][index] == sq) {
            temp_piece_num = index;
            break;
        }
    }

    assert(temp_piece_num != -1);

    pos->piece_num[piece]--;
    pos->piece_list[piece][temp_piece_num] = pos->piece_list[piece][pos->piece_num[piece]]; 
}

static void AddPiece (const int sq, S_BOARD *pos, const int piece) {
    assert(PieceValid(piece));
    assert(SqOnBoard(sq));

    int col = PieceColour[piece];
    HASH_PIECE(piece, sq); // hash piece in
    pos->pieces[sq] = piece;
    if (PieceBig[piece]) {
        pos->big_piece[col] ++;
        if(PieceMajor[piece]) {
            pos->maj_piece[col]++;
        } else {
             pos->min_piece[col]++;
        }
    } else {
        SETBIT(pos->pawns[col], SQ64(sq));
        SETBIT(pos->pawns[BOTH], SQ64(sq));
    }
    pos->material[col] += PieceValue[piece];
    pos->piece_list[piece][pos->piece_num[piece]++] = sq;
}

static void MovePiece(const int from, const int to, S_BOARD *pos) {
    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    int index = 0;
    int piece = pos->pieces[from];
    int col = PieceColour[piece];

    int temp_piece_num = false;

    HASH_PIECE(piece,from); // hash out piece from from sq
    pos->pieces[from] = EMPTY;
    HASH_PIECE(piece,to); // hash in piece moved to to sq
    pos->pieces[to] = piece;

    if(!PieceBig[piece]) {
        CLRBIT(pos->pawns[col], SQ64(from));
        CLRBIT(pos->pawns[BOTH], SQ64(from));
        SETBIT(pos->pawns[col], SQ64(to));
        SETBIT(pos->pawns[BOTH], SQ64(to));
    }

    for(index = 0; index < pos->piece_num[piece]; ++index) {
        if(pos->piece_list[piece][index] == from) {
            pos->piece_list[piece][index] = to;
            #ifdef DEBUG
            temp_piece_num = true;
            #endif
            break;
        }
    }
    assert(temp_piece_num);

}

int MakeMove (S_BOARD *pos, int move) {
    assert(CheckBoard(pos));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));
    assert(SideValid(side));
    assert(PieceValid(pos->pieces[from]));

    pos->history[pos->hist_half_moves].posKey = pos->pos_key;

    if (move & MFLAGEP) {
        if(side == WHITE) {
            ClearPiece(to - 10, pos);
        } else {
            ClearPiece(to + 10, pos);
        }
    } else if (move & MFLAGCASTLE) {
        switch(to) {
            case C1:
                MovePiece(A1, D1, pos);
                break;
            case C8:
                MovePiece(A8, D8, pos);
                break;
            case G1:
                MovePiece(H1, F1, pos);
                break;
            case G8:
                MovePiece(H8, F8, pos);
                break;
            default: assert(false); break;
        }
    }

    if(pos->enPas != NO_SQ) HASH_EP; // hash out the old EP square
    HASH_CA; // hash out the old castle perm

    pos->history[pos->hist_half_moves].move = move;
    pos->history[pos->hist_half_moves].fifty_move_count = pos->fifty_move_count;
    pos->history[pos->hist_half_moves].enPas = pos->enPas;
    pos->history[pos->hist_half_moves].castle_perm = pos->castle_perm;

    pos->castle_perm &= CastlePerm[from];
    pos->castle_perm &= CastlePerm[to];
    pos->enPas = NO_SQ;

    HASH_CA; // hash in the new castle perm

    int captured = CAPTURED(move);
    pos->fifty_move_count++;

    if(captured != EMPTY) {
        assert(PieceValid(captured));
        ClearPiece(to,pos);
        pos->fifty_move_count = 0;
    }

    pos->hist_half_moves++;
    pos->half_moves++;

    if(PiecePawn[pos->pieces[from]]) {
        pos->fifty_move_count = 0;
        if(move & MFLAGPS) {
            if (side==WHITE) {
                pos->enPas = from + 10;
                assert(RanksBrd[pos->enPas] == RANK_3);
            } else {
                pos->enPas = from - 10;
                assert(RanksBrd[pos->enPas] == RANK_6);
            }
            HASH_EP; //hash in the new enPas square
        }
    }

    MovePiece(from, to, pos);

    int promoted_piece = PROMOTED(move);
    if(promoted_piece != EMPTY) {
        assert(PieceValid(promoted_piece) && !PiecePawn[promoted_piece]);
        ClearPiece(to, pos);
        AddPiece(to, pos, promoted_piece);
    }

    // if king has moved change the king sq
    if(PieceKing[pos->pieces[to]]) { 
        pos->KingSq[pos->side] = to;
    }

    pos->side ^= 1; // change side colour
    HASH_SIDE; // hash in the new side

    assert(CheckBoard(pos));

    // if the king is attacked (illegal move) take back the move
    if(SqAttacked(pos->KingSq[side], pos->side, pos)) {
        TakeMove(pos);
        return false;
    }

    return true;
}

// take back a move
void TakeMove(S_BOARD *pos) {
    assert(CheckBoard(pos));

    pos->hist_half_moves--;
    pos->half_moves--;

    int move = pos->history[pos->hist_half_moves].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    assert(SqOnBoard(from));
    assert(SqOnBoard(to));

    if(pos->enPas != NO_SQ) HASH_EP; // hash out the new EP square
    HASH_CA; // hash out the new castle perm

    pos->castle_perm = pos->history[pos->hist_half_moves].castle_perm;
    pos->fifty_move_count = pos->history[pos->hist_half_moves].fifty_move_count;
    pos->enPas = pos->history[pos->hist_half_moves].enPas;

    if(pos->enPas != NO_SQ) HASH_EP; // hash in old EP
    HASH_CA; // hash in the old castle perm

    pos->side ^= 1; // change side colour
    HASH_SIDE; // hash in the old side

    if (MFLAGEP & move) {
        if(pos->side == WHITE) {
            AddPiece(to - 10, pos, bP);
        } else {
            AddPiece(to + 10, pos, wP);
        }
    } else if (MFLAGCASTLE & move) {
        switch(to) {
            case C1:
                MovePiece(D1, A1, pos);
                break;
            case C8:
                MovePiece(D8, A8, pos);
                break;
            case G1:
                MovePiece(F1, H1, pos);
                break;
            case G8:
                MovePiece(F8, H8, pos);
                break;
            default: assert(false); break;
        }
    }

    MovePiece(to, from, pos);

    if(PieceKing[pos->pieces[from]]) { 
        pos->KingSq[pos->side] = from;
    }
    
    int captured = CAPTURED(move);
    if(captured != EMPTY) {
        assert(PieceValid(captured));
        AddPiece(to, pos, captured);
    }

    int promoted_piece = PROMOTED(move);
    if(promoted_piece != EMPTY) {
        assert(PieceValid(promoted_piece) && !PiecePawn[promoted_piece]);
        ClearPiece(from, pos);
        AddPiece(from, pos, (PieceColour[promoted_piece] == WHITE ? wP : bP));
    }

    assert(CheckBoard(pos));

}