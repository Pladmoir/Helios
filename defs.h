#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
#include <assert.h>

// #define NDEBUG  // uncomment to disable assertion checks

typedef unsigned long long U64; // using a bits for board representation (64 bits = 64 squares)

#define NAME "Helios 1.0"
#define BOARD_SQR_NUM 120

#define MAXGAMEMOVES 2048

enum { EMPTY, wP, wH, wB, wR, wQ, wK, bP, bH, bB, bQ, bK }; 
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum { WHITE, BLACK, BOTH };
enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1, 
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 }; // castling (format: colour - side being castled) represented by bits

///////// Structures ////////////

// structure to keep record of the history of the chess game
typedef struct {
    int move;
    int castle_perm;
    int enPas;
    int fifty_move_count;
    U64 posKey;
} S_UNDO;

// structure for the chess board
typedef struct {
    int pieces[BOARD_SQR_NUM];
    U64 pawns[3]; // 2 for colors 1 for combined (acts like an array[64])

    int KingSq[2];

    int side; // which side is currently going
    int enPas; // keep track of en-passent square
    int fifty_move_count;

    int half_moves;
    int hist_half_moves; // used for repetition rule

    int castle_perm;

    U64 pos_key; 

    int piece_num[13]; // record of piece totals, piece determined by enum above
    int big_piece[3]; // non-pawn pieces for each colour
    int maj_piece[3]; // rooks and queens for each colour
    int min_piece[3]; // bishops and knights for each colour

    S_UNDO history[MAXGAMEMOVES]; //array of all moves

    int piece_list[13][10]; // example of use [wH][0] = E1
    
} S_BOARD;

///////// Macros ////////////

#define FR2SQ(f,r) ( (21 + f) + (r * 10)) // determines index in the 120 array based on file and rank
#define SQ64(sq120) Sq64ToSq120[sq120]

///////// Globals ////////////

extern int Sq120ToSq64[BOARD_SQR_NUM];
extern int Sq64ToSq120[64];

///////// Functions ////////////

// init.c
extern void init();

//bitboards.c

extern void PrintBitBoard(U64 bb);

#endif