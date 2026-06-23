#include "defs.h"
#include <stdbool.h>

const int HorseDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int RkDir[4] = {-1, -10, 1, 10};
const int BiDir[4] = { -9, -11, 11, 9};
const int KiDir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

// checks if a square is under attack
int SqAttacked (const int sq, const int attack_side, const S_BOARD *pos) {
    int piece, index, temp_sq, dir;

    assert(SqOnBoard(sq));
    assert(SideValid(attack_side));
    assert(CheckBoard(pos));

    // pawns

    if (attack_side == WHITE) {
        if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
            return true;
        } 
    } else {
        if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
            return true;
        }
    }

    // knights
    for (index = 0; index < 8; ++index) {
        piece = pos->pieces[sq + HorseDir[index]];
        if(IsHorse(piece) && PieceColour[piece] == attack_side) {
            return true;
        }
    }

    // rooks, queens
    for(index = 0; index < 4; ++index) {
        dir = RkDir[index];
        temp_sq = sq + dir;
        piece = pos->pieces[temp_sq];
        while(piece != OFFBOARD) {
            if(piece != EMPTY) {
                if(IsRQ(piece) && PieceColour[piece] == attack_side) {
                    return true;
                }
                break;
            }
            temp_sq += dir;
            piece = pos->pieces[temp_sq];
        }
    }

    // bishops, queen
    for(index = 0; index < 4; ++index) {
        dir = BiDir[index];
        temp_sq = sq + dir;
        piece = pos->pieces[temp_sq];
        while(piece != OFFBOARD) {
            if(piece != EMPTY) {
                if(IsBQ(piece) && PieceColour[piece] == attack_side) {
                    return true;
                }
                break;
            }
            temp_sq += dir;
            piece = pos->pieces[temp_sq];
        }
    }

    // king
    for (index = 0; index < 8; ++index) {
        piece = pos->pieces[sq + KiDir[index]];
        if(IsKing(piece) && PieceColour[piece] == attack_side) {
            return true;
        }
    }

    return false;

}