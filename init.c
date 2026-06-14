#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

// generates a 64 bit random number
#define RAND_64 (   (U64)rand() | \
                    (U64)rand() << 15 | \
                    (U64)rand() << 30 | \
                    (U64)rand() << 45 | \
                    ((U64)rand() & 0xf) << 60  )



int Sq120ToSq64[BOARD_SQR_NUM];
int Sq64ToSq120[64];

U64 SetMask[64];
U64 ClearMask[64];

U64 PieceKeys[13][120]; // key based on pieces
U64 SideKey; // key for side 
U64 CastleKey[16]; // key for catle side (4 bits)

int FilesBrd[BOARD_SQR_NUM];
int RanksBrd[BOARD_SQR_NUM];

// inititalize the file and rank arrays
void InitFilesRanksBrd() {
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;

    for(index = 0; index < BOARD_SQR_NUM; ++index) {
        FilesBrd[index] = OFFBOARD;
        RanksBrd[index] = OFFBOARD;
    }

    for (rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file, rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

// fill out PieceKeys, SideKey, and CastleKey with random 64 bit numbers
void InitHashKeys() {

    int index = 0;
    int index2 = 0;
    for (index = 0; index < 13; ++index) {
        for (index2 = 0; index2 < 120; ++index2) {
            PieceKeys[index][index2] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for(index = 0; index < 16; ++index) {
        CastleKey[index] = RAND_64;
    }

}

// Initialzie SetMask and ClearMask bit arrays
void InitBitMasks() {
    int index = 0;
    for (index = 0; index < 64; ++index) {
        SetMask[index] = 0ULL;
        ClearMask[index] = 0ULL;
    }

    for (index = 0; index < 64; ++index) {
        SetMask[index] = (1ULL << index);
        ClearMask[index] = ~SetMask[index];
    }
}

// Initialize both 120 and 64 board conversion array
static void InitSq120To64() {

    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;
    for (index = 0; index < BOARD_SQR_NUM; ++index) {
        Sq120ToSq64[index] = 65;
    }

    for(index = 0; index< 64; ++index) {
        Sq64ToSq120[index] = 120;
    }

    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file,rank);
            Sq64ToSq120[sq64] = sq; // conversion array from 64 square to 120 square board
            Sq120ToSq64[sq] = sq64; // conversion array from 120 square to 64 square
            sq64++;
        }
    }
}

void init() {
    InitSq120To64();
    InitBitMasks();
    InitHashKeys();
    InitFilesRanksBrd();
}