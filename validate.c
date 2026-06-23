#include "defs.h"

int SqOnBoard(const int sq) {
    return FilesBrd[sq] == OFFBOARD ? 0 : 1;   
}

int SideValid(const int side) {
    return (side == WHITE || side == BLACK) ? 1 : 0;   
}

int FileRankValid(const int fr) {
    return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int PieceValidEmpty(const int piece) {
    return (piece >= EMPTY && piece <= bK) ? 1 : 0;
}

int PieceValid(const int piece) {
    return (piece >= wP && piece <= bK) ? 1 : 0;
}