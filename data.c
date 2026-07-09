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

int PiecePawn[13] = {false, true, false, false, false, false, false, true, false, false, false, false, false};
int PieceKnight[13] = {false, false, true, false, false, false, false, false, true, false, false, false, false};
int PieceKing[13] = {false, false, false, false, false, false, true, false, false, false, false, false, true};
int PieceRookQueen[13] = {false, false, false, false, true, true, false, false, false, false, true, true, false};
int PieceBishopQueen[13] = {false, false, false, true, false, true, false, false, false, true, false, true, false};
int PieceSlides[13] = {false, false, false, true, true, true, false, false, false, true, true, true, false};

// array to mirrow the inputed sq to black's pov
int Mirror64[64] = {
    56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
	48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
	40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
	32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
	24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
	16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
	8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
	0	,	1	,	2	,	3	,	4	,	5	,	6	,	7		
};