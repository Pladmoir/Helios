#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

void MirrorEvalTest(S_BOARD *pos) {
    FILE *file;
    file = fopen("mirror.epd", "r");
    char lineIn [1024];
    int ev1 = 0; int ev2 = 0;
    int positions = 0;
    if(file == NULL) {
        printf("File not found\n");
        return;
    } else {
        while(fgets (lineIn, 1024, file) != NULL) {
            ParseFen(lineIn, pos);
            positions++;
            ev1 = EvalPosition(pos);
            MirrorBoard(pos);
            ev2 = EvalPosition(pos);

            if(ev1 != ev2) {
                printf("\n\n\n");
                ParseFen(lineIn, pos);
                PrintBoard(pos);
                PrintBoard(pos);
                printf("\n\nMirror Fail: ev1:%d ev2:%d \n%s\n", ev1, ev2, lineIn);
                getchar();
                return;
            }
            if((positions % 1000) == 0) {
                printf("position %d\n", positions);
            }
            memset(&lineIn[0],0, sizeof(lineIn)); // clear linein for next iteration
        }
        printf("\nMirror Test Completed!\n");
    }
}