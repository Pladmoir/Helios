#include <stdbool.h>
#include "defs.h"


char PieceChar[] = ".PNBRQKpnbrqk";
char SideChar[] = "wb-";
char RankChar[] = "12345678";
char FileChar[] = "abcdefgh";

int PieceBig[13] = {false, false, true, true, true, true, true, false, true, true, true, true, true};
int PieceMajor[13] = {false, false, false, false, true, true, true, false, false, false, true, true, true};
int PieceMinor[13] = {false, false, true, true, false, false, false, false, true, true, false, false, false};
int PieceValue[13] = {0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000};
int PieceColour[13] = {BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};
