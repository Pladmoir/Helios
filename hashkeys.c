#include "defs.h"

U64 GeneratePositionKey (const S_BOARD *pos) {
    int sq = 0;
    U64 finalKey = 0;
    int piece = EMPTY;
    
    // pieces

    // loops through all squares and generates a final position key
    for (sq = 0; sq < BOARD_SQR_NUM; ++sq) {
        piece = pos->pieces[sq];
        if(piece != NO_SQ && piece != EMPTY) {
            assert(piece >= wP && piece <= bK); // assert piece value is in valid range
            finalKey ^= PieceKeys[piece][sq];
        }
    }

    // if its white turn also add in the side key to our final key
    if (pos->side == WHITE) {
        finalKey ^= SideKey;
    }

    // if the position has an enpassent possible modify the hashkey
    if (pos->enPas != NO_SQ) {
        assert(pos->enPas >= 0 && pos->enPas < BOARD_SQR_NUM);
        finalKey ^= PieceKeys[EMPTY][pos->enPas];
    }

    assert(pos->castle_perm >= 0 && pos->castle_perm <= 15);

    finalKey ^= CastleKey[pos->castle_perm];

    return finalKey;
}