#include "defs.h"

int Sq120ToSq64[BOARD_SQR_NUM];
int Sq64ToSq120[64];

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
}